#include "Server.h"
#include "GameState.h"
#include "InputCommand.h"
#include "Level.h"
#include "RoundItems.h"
#include "Simulation.h"
#include "Snapshot.h"
#include "TrafficLightEntity.h"
#include "gameLevels.h"
#include <chrono>
#include <cstring>
#include <deque>
#include <enet/enet.h>
#include <iostream>
#include <thread>
#include <cstdlib>

void runServer() {

  ENetAddress myAddress;
  myAddress.host = ENET_HOST_ANY; // listen on all network interfaces
  myAddress.port = 7777;
  ENetHost *server =
    enet_host_create(&myAddress, 32, 2, 0, 0); // up to 32 peers, 2 channels
  if (server == nullptr) {
    std::cout << "ENet failed to create host\n";
    return;
  }
  std::cout << "Server listening on port 7777\n";

  //how many levels are in the game
  int gameLevelsCount;
  gameLevelsCount = levelList.size();
  
  // when doing rand() % n, it will be from 0 to n - 1. because size() is the
  // count of how many levels, 1 level will return 1, even though its index 0.
  // So gameLevelsCount is 1 too high to index from, and rand() % n wants 1 more
  // than the highest value, so it all works well.
  int randomLevelIndex = rand() % gameLevelsCount;
  std::string levelPath = levelList[randomLevelIndex];
  
  GameState gameState;
  Level level = loadLevel(levelPath, nullptr, nullptr);

  PlayerState player0;
  PlayerState player1;
  // ISSUE randomise spawns
  if (level.spawns.size() >= 2) {
    player0.position = level.spawns[0];
    player1.position = level.spawns[1];
  }
  gameState.players.push_back(player0);
  gameState.players.push_back(player1);

  giveRoundItems(gameState.players[0]);
  giveRoundItems(gameState.players[1]);

  std::vector<std::deque<InputCommand>> pendingCommands(2);
  std::vector<InputCommand> lastApplied(2);
  std::vector<unsigned int> highestRecieved(2, 0);
  std::vector<unsigned int> lastAppliedSequence(2, 0);

  RoundPhase previousPhase = RoundPhase::Active;

  const float tickRate = 1.0f / 60.0f; // 60hz
  ENetEvent event;
  int tickCounter = 0;
  int nextPlayerIndex = 0;

  auto nextTick = std::chrono::steady_clock::now();

  // tick loop:
  while (true) {
    nextTick += std::chrono::microseconds(16667); // 1/60 in microseconds
    // this is the constant loop of running the netcode
    // returns a non-zero value if an event has happened and stores in event var

    // first drain network events
    while (enet_host_service(server, &event, 0) >
           0) { // the 0 in enet_host_service means dont wait at all (we dont
                // want to delay actual game simulation)
      switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT: {
        int playerIndex = nextPlayerIndex;
        event.peer->data =
            (void *)(intptr_t)playerIndex; // remember who this peer is
        nextPlayerIndex++;
        std::cout << "Client connected as player " << playerIndex << "\n";

        unsigned char buffer[1 + sizeof(WelcomeMessage)];
        buffer[0] = (unsigned char)
            MessageType::Welcome; // set the first bit as the message type bit
                                  // (welcome = 1)
        WelcomeMessage welcome;
        welcome.playerIndex = playerIndex;
        welcome.currentLevelIndex = randomLevelIndex;
        memcpy(buffer + 1, &welcome, sizeof(WelcomeMessage));

        ENetPacket *packet = enet_packet_create(
            buffer, sizeof(buffer), ENET_PACKET_FLAG_RELIABLE); // reliable
        enet_peer_send(event.peer, 0, packet);
        break;
      }

        // event.peer->data is a spare void* enet gives u to put a pointer in.
        // (void*)(intptr_t) is a way to put a int in a pointer slot

      case ENET_EVENT_TYPE_RECEIVE: {
        if (event.packet->dataLength == sizeof(CommandPacket)) {
          int playerIndex = (int)(intptr_t)event.peer->data;
          if (playerIndex < (int)pendingCommands.size()) {
            CommandPacket incoming;
            memcpy(&incoming, event.packet->data, sizeof(CommandPacket));

            for (int i = 0; i < 3; i++) {
              const InputCommand &c = incoming.commands[i];
              if (c.sequence == 0)
                continue;
              if (c.sequence <= highestRecieved[playerIndex])
                continue;

              pendingCommands[playerIndex].push_back(c);
              highestRecieved[playerIndex] = c.sequence;
            }

            while (pendingCommands[playerIndex].size() > 8) {
              pendingCommands[playerIndex]
                  .pop_front(); // discard the old stale commands
            }
          }
        }
        enet_packet_destroy(
            event.packet); // we own the packet, so we are responsible for
                           // destroying it (dont want to mem leak)
        break;
      }

      case ENET_EVENT_TYPE_DISCONNECT: {
        std::cout << "A client disconnected\n";
        break;
      }

      default: {
        break;
      }
      }
    }

    // SIMULATE one tick
    
    gameState.colliders.clear();
    for (GameObject &obj : level.objects) {
      if (obj.collidable) {
        gameState.colliders.push_back(obj.getAABB());
      }
    }

    //reset stats as active ones will be set again on this tick
    for (PlayerState &p : gameState.players) {
      resetPlayerStats(p);
    }

    updateWorld(gameState, level.spawns, tickRate);

    //evaluate if round has just become active from RoundOver
    if (previousPhase == RoundPhase::RoundOver &&
        gameState.phase == RoundPhase::Active) {
      giveRoundItems(gameState.players[0]);
      giveRoundItems(gameState.players[1]);
    }
    previousPhase = gameState.phase;

    for (int i = 0; i < (int)gameState.players.size(); i++) {
      InputCommand command;

      if (!pendingCommands[i].empty()) {
        command = pendingCommands[i].front();
        pendingCommands[i].pop_front();
        lastApplied[i] = command;
        lastAppliedSequence[i] = command.sequence;
      } else {
        // no input coming in, starved. hold movement and drop edges so nothing
        // repeats
        command = lastApplied[i];
        command.primaryPressed = false;
        command.secondaryPressed = false;
        command.dash = false;
        command.jump = false;
        command.equipWeapon = false;
        command.equipAbility = false;
        // dont advance lastAppliedSequence bc nothing new was consumed
      }

      processPlayerInput(gameState, i, command, tickRate);
    }

    // build a snapshot:
    Snapshot snapshot;
    snapshot.tick = tickCounter;
    snapshot.playerCount = (int)gameState.players.size();
    for (int i = 0; i < snapshot.playerCount && i < 2; i++) {
      snapshot.players[i].lastAppliedSequence =
          lastAppliedSequence[i]; // telling player that the below info is from
                                  // this (N) snapshot
      snapshot.players[i].position = gameState.players[i].position;
      snapshot.players[i].velocity = gameState.players[i].velocity;
      snapshot.players[i].dashDirection = gameState.players[i].dashDirection;
      snapshot.players[i].dashTimeLeft = gameState.players[i].dashTimeLeft;
      snapshot.players[i].dashRechargeTimer =
          gameState.players[i].dashRechargeTimer;
      snapshot.players[i].dashCharges = gameState.players[i].dashCharges;
      snapshot.players[i].grounded = gameState.players[i].grounded;
      snapshot.players[i].lookDirection = gameState.players[i].lookDirection;
      snapshot.players[i].health = gameState.players[i].health;
      snapshot.players[i].sliding = gameState.players[i].sliding;
    }
    snapshot.roundWins[0] = gameState.roundWins[0];
    snapshot.roundWins[1] = gameState.roundWins[1];
    snapshot.phase = (int)gameState.phase;

    snapshot.entityCount = 0;
    for (WorldEntity *entity : gameState.worldEntities) {
      if (snapshot.entityCount >= 16)
        break; // dont go over the limit

      // ISSUE - why are we hardcoding the rot and colour states into the
      // snapshot
      // Its because 'entity' is a worldEntity, and doesnt have the
      // TrafficLightEntities 'getRotationY' methods etc
      EntitySnapshot &e = snapshot.entities[snapshot.entityCount];
      e.type = (int)entity->getType();
      e.position = entity->position;
      e.rotationY = 0.0f;
      e.isRed = false;
      e.isAmber = false;

      // ISSUE - its hardcoded how it simulates each entity. they should have a method
      if (entity->getType() == EntityType::TrafficLight) {
        TrafficLightEntity *tl = static_cast<TrafficLightEntity *>(entity);
        e.rotationY = tl->getRotationY();
        e.isRed = tl->getIsRed();
        e.isAmber = tl->getIsAmber();
      }

      snapshot.entityCount++;
    }

    unsigned char buffer[1 + sizeof(Snapshot)];
    buffer[0] = (unsigned char)MessageType::Snapshot;
    memcpy(buffer + 1, &snapshot, sizeof(Snapshot));

    ENetPacket *packet = enet_packet_create(buffer, sizeof(buffer), 0);
    enet_host_broadcast(
        server, 0, packet); // sends to every connected peer. 0 means unreliable

    // debug output:
    tickCounter++;
    // we do AND 1 == 0 to stop debug output. hardcoded
    if (tickCounter % 60 == 0 && 1 == 0) {
      std::cout << "x: " << gameState.players[0].position.x
                << "  y: " << gameState.players[0].position.y
                << "  z: " << gameState.players[0].position.z
                << "  grounded: " << gameState.players[0].grounded << "\n";
    }

    // sleep so we stay at 60Hz
    std::this_thread::sleep_until(nextTick);
  }

  enet_host_destroy(server);
}

#include "Server.h"
#include "GameState.h"
#include "Level.h"
#include "Simulation.h"
#include "InputCommand.h"
#include "Snapshot.h"
#include "RoundItems.h"
#include <enet/enet.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>

void runServer() {
    ENetAddress myAddress;
    myAddress.host = ENET_HOST_ANY; //listen on all network interfaces
    myAddress.port = 7777;
    ENetHost* server = enet_host_create(&myAddress, 32, 2, 0, 0); //up to 32 peers, 2 channels
    if (server == nullptr) { 
        std::cout << "ENet failed to create host\n";
        return;
    }
    std::cout << "Server listening on port 7777\n";


    GameState gameState;
    Level level = loadLevel("test1v1level.level.json", nullptr, nullptr);

    PlayerState player0;
    PlayerState player1;
    if (level.spawns.size() >= 2) {
        player0.position = level.spawns[0];
        player1.position = level.spawns[1];
    }
    gameState.players.push_back(player0);
    gameState.players.push_back(player1);

    giveRoundItems(gameState.players[0]);
    giveRoundItems(gameState.players[1]);

    std::vector<InputCommand> commands(2); //one command slot per player

    RoundPhase previousPhase = RoundPhase::Active;

    const float tickRate = 1.0f / 60.0f; // 60hz
    ENetEvent event;
    int tickCounter = 0;
    int nextPlayerIndex = 0;

    std::vector<unsigned int> lastAppliedSequence(2, 0);

    auto nextTick = std::chrono::steady_clock::now();

    //tick loop:
    while (true) {
        nextTick += std::chrono::microseconds(16667); // 1/60 in microseconds
        //this is the constant loop of running the netcode
        //returns a non-zero value if an event has happened and stores in event var

        //first drain network events
        while (enet_host_service(server, &event, 0) > 0) { //the 0 in enet_host_service means dont wait at all (we dont want to delay actual game simulation)
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT: {
                    int playerIndex = nextPlayerIndex;
                    event.peer->data = (void*)(intptr_t)playerIndex; //remember who this peer is
                    nextPlayerIndex++;
                    std::cout << "Client connected as player " << playerIndex << "\n";

                    unsigned char buffer[1 + sizeof(WelcomeMessage)];
                    buffer[0] = (unsigned char)MessageType::Welcome; //set the first bit as the message type bit (welcome = 1)
                    WelcomeMessage welcome;
                    welcome.playerIndex = playerIndex;
                    memcpy(buffer + 1, &welcome, sizeof(WelcomeMessage));

                    ENetPacket* packet = enet_packet_create(buffer, sizeof(buffer), ENET_PACKET_FLAG_RELIABLE); //reliable
                    enet_peer_send(event.peer, 0, packet);
                    break;
                }
                
                //event.peer->data is a spare void* enet gives u to put a pointer in. (void*)(intptr_t) is a way to put a int in a pointer slot

                case ENET_EVENT_TYPE_RECEIVE: {
                    if (event.packet->dataLength == sizeof(CommandPacket)) {
                        int playerIndex = (int)(intptr_t)event.peer->data;
                        if (playerIndex < (int)commands.size()) {
                            CommandPacket incoming;
                            memcpy(&incoming, event.packet->data, sizeof(CommandPacket));

                            for (int i = 0; i < 3; i++) {
                                const InputCommand& c = incoming.commands[i];
                                if (c.sequence == 0) continue; //0 means its an empty slot
                                if (c.sequence <= lastAppliedSequence[playerIndex]) continue; //already seen

                                //merge: edge inputs OR together so none are lost
                                commands[playerIndex].primaryPressed |= c.primaryPressed;
                                commands[playerIndex].secondaryPressed |= c.secondaryPressed;
                                commands[playerIndex].dash |= c.dash;
                                commands[playerIndex].jump |= c.jump;
                                commands[playerIndex].equipWeapon |= c.equipWeapon;
                                commands[playerIndex].equipAbility|= c.equipAbility;

                                //continuous state: newest wins
                                commands[playerIndex].moveForward = c.moveForward;
                                commands[playerIndex].moveBack = c.moveBack;
                                commands[playerIndex].moveLeft = c.moveLeft;
                                commands[playerIndex].moveRight = c.moveRight;
                                commands[playerIndex].crouch = c.crouch;
                                commands[playerIndex].primaryHeld = c.primaryHeld;
                                commands[playerIndex].secondaryHeld = c.secondaryHeld;
                                commands[playerIndex].lookDirection = c.lookDirection;
                                commands[playerIndex].sequence = c.sequence;

                                lastAppliedSequence[playerIndex] = c.sequence;
                            }
                        }
                    }
                    enet_packet_destroy(event.packet); //we own the packet, so we are responsible for destroying it (dont want to mem leak)
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

        //simulate one tick
        gameState.colliders.clear();
        for (GameObject& obj : level.objects) {
            if (obj.collidable) {
                gameState.colliders.push_back(obj.getAABB());
            }
        }

        for (PlayerState& p : gameState.players) {
            resetPlayerStats(p);
        }

        updateWorld(gameState, level.spawns, tickRate);


        if (previousPhase == RoundPhase::RoundOver && gameState.phase == RoundPhase::Active) {
            giveRoundItems(gameState.players[0]);
            giveRoundItems(gameState.players[1]);
        }
        previousPhase = gameState.phase;


        for (int i = 0; i < (int)gameState.players.size(); i++) {
            processPlayerInput(gameState, i, commands[i], tickRate);
        }


        //build a snapshot:
        Snapshot snapshot;
        snapshot.tick = tickCounter;
        snapshot.playerCount = (int)gameState.players.size();
        for (int i = 0; i < snapshot.playerCount && i < 2; i++) {
            snapshot.players[i].position = gameState.players[i].position;
            snapshot.players[i].lookDirection = gameState.players[i].lookDirection;
            snapshot.players[i].health = gameState.players[i].health;
            snapshot.players[i].sliding = gameState.players[i].sliding;
            snapshot.players[i].dashTimeLeft = gameState.players[i].dashTimeLeft;
        }
        snapshot.roundWins[0] = gameState.roundWins[0];
        snapshot.roundWins[1] = gameState.roundWins[1];
        snapshot.phase = (int)gameState.phase;

        unsigned char buffer[1 + sizeof(Snapshot)];
        buffer[0] = (unsigned char)MessageType::Snapshot;
        memcpy(buffer + 1, &snapshot, sizeof(Snapshot));

        ENetPacket* packet = enet_packet_create(buffer, sizeof(buffer), 0);
        enet_host_broadcast(server, 0, packet); //sends to every connected peer. 0 means unreliable

        //clear the edges so they dont repeat:
        for (InputCommand& c : commands) {
            c.primaryPressed = false;
            c.secondaryPressed = false;
            c.dash = false;
            c.jump = false;
            c.equipWeapon = false;
            c.equipAbility = false;
        }


        //debug output:
        tickCounter++;
        if (tickCounter % 60 == 0) {
            std::cout << "x: " << gameState.players[0].position.x
                      << "  y: " << gameState.players[0].position.y
                      << "  z: " << gameState.players[0].position.z
                      << "  grounded: " << gameState.players[0].grounded << "\n";
        }

        //sleep
        std::this_thread::sleep_until(nextTick);
    }

    enet_host_destroy(server);
}
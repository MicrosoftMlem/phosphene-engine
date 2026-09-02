#include "NetworkClient.h"
#include <iostream>
#include <cstring>

NetworkClient::NetworkClient() {
    host = enet_host_create(nullptr, 1, 2, 0, 0);
    peer = nullptr;
    if (host == nullptr) {
        std::cout << "Failed to create client host\n";
    }
}

NetworkClient::~NetworkClient() {
    if (host != nullptr) {
        enet_host_destroy(host);
    }
}

bool NetworkClient::connectToServer(const char* hostName, int port) {
    ENetAddress address;
    enet_address_set_host(&address, hostName);
    address.port = port;

    peer = enet_host_connect(host, &address, 2, 0);
    if (peer == nullptr) {
        std::cout << "No available peers\n";
        return false;
    }

    ENetEvent event;
    if (enet_host_service(host, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "Connected to server\n";
        return true;
    }

    //shouldnt reach here if we had a successful connection
    enet_peer_reset(peer);
    peer = nullptr;
    std::cout << "Connection failed\n";
    return false;
}

unsigned int NetworkClient::sendCommand(InputCommand command) {
    if (peer == nullptr) return 0;

    command.sequence = nextSequence;
    nextSequence++;

    //shift the history left, with the newest on the end
    recentCommands[0] = recentCommands[1];
    recentCommands[1] = recentCommands[2];
    recentCommands[2] = command;

    CommandPacket outgoing;
    outgoing.commands[0] = recentCommands[0];
    outgoing.commands[1] = recentCommands[1];
    outgoing.commands[2] = recentCommands[2];

    ENetPacket* packet = enet_packet_create(&outgoing, sizeof(CommandPacket), 0);
    enet_peer_send(peer, 0, packet);

    return command.sequence;
}


void NetworkClient::poll(float currentTime) {
    if (host == nullptr) return;
    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            unsigned char type = ((unsigned char*)event.packet->data)[0]; //first bit defines type
            unsigned char* payload = (unsigned char*)event.packet->data + 1;

            if (type == (unsigned char)MessageType::Snapshot) {
                TimedSnapshot ts;
                memcpy(&ts.snapshot, payload, sizeof(Snapshot));
                ts.recievedTime = currentTime;
                snapshotBuffer.push_back(ts);
                while (snapshotBuffer.size() > 32) snapshotBuffer.pop_front(); //free stale ones

                latestSnapshot = ts.snapshot;
                snapshotRecieved = true;
            }
            else if (type == (unsigned char)MessageType::Welcome) {
                WelcomeMessage welcome;
                memcpy(&welcome, payload, sizeof(WelcomeMessage));
                myPlayerIndex = welcome.playerIndex;
            }
            enet_packet_destroy(event.packet); //we handle packets so we gotta destroy them once done with them to stop mem leak
        }
    }
}


bool NetworkClient::getInterpolatedPlayer(int playerIndex, float renderTime, PlayerSnapshot& out) const {
    if (snapshotBuffer.size() < 2) return false;

    for (size_t i = 0; i + 1 < snapshotBuffer.size(); i++) {
        const TimedSnapshot& earlier = snapshotBuffer[i];
        const TimedSnapshot& later = snapshotBuffer[i + 1];

        if (earlier.recievedTime <= renderTime & renderTime <= later.recievedTime) {
            float span = later.recievedTime - earlier.recievedTime;
            float alpha = (span > 0.0f) ? (renderTime - earlier.recievedTime) / span : 0.0f; //lowest is 0

            if (playerIndex >= earlier.snapshot.playerCount || playerIndex >= later.snapshot.playerCount) return false;

            const PlayerSnapshot& a = earlier.snapshot.players[playerIndex];
            const PlayerSnapshot& b = later.snapshot.players[playerIndex];

            out.position = glm::mix(a.position, b.position, alpha);
            out.lookDirection = glm::mix(a.lookDirection, b.lookDirection, alpha);
            out.health = b.health;
            out.sliding = b.sliding;
            return true;
        }
    }

    return false; //renderTime outside the buffer
}

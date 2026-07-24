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


void NetworkClient::poll() {
    if (host == nullptr) return;
    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            unsigned char type = ((unsigned char*)event.packet->data)[0]; //first bit defines type
            unsigned char* payload = (unsigned char*)event.packet->data + 1;

            if (type == (unsigned char)MessageType::Snapshot) {
                memcpy(&latestSnapshot, payload, sizeof(Snapshot));
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
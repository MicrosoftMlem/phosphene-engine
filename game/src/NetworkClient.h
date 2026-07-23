#pragma once
#include <enet/enet.h>
#include "InputCommand.h"
#include "Snapshot.h"

class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();

    bool connectToServer(const char* host, int port);
    void sendCommand(const InputCommand& command);
    void poll();
    
    bool hasSnapshot() const { return snapshotRecieved; }
    const Snapshot& getSnapshot() const { return latestSnapshot; }
    int getPlayerIndex() const { return myPlayerIndex; }

private:
    ENetHost* host;
    ENetPeer* peer;

    Snapshot latestSnapshot;
    bool snapshotRecieved = false;

    int myPlayerIndex = -1;
};
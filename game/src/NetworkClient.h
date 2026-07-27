#pragma once
#include <enet/enet.h>
#include <deque>
#include "InputCommand.h"
#include "Snapshot.h"



class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();

    bool connectToServer(const char* host, int port);
    unsigned int sendCommand(InputCommand command);
    void poll(float currentTime);
    
    bool hasSnapshot() const { return snapshotRecieved; }
    const Snapshot& getSnapshot() const { return latestSnapshot; }
    int getPlayerIndex() const { return myPlayerIndex; }
    
    unsigned int getNextSequence() const { return nextSequence; }

    bool getInterpolatedPlayer(int playerIndex, float renderTime, PlayerSnapshot& out) const;

    struct TimedSnapshot {
        Snapshot snapshot;
        float recievedTime;
    };

private:
    ENetHost* host;
    ENetPeer* peer;

    Snapshot latestSnapshot;
    bool snapshotRecieved = false;

    InputCommand recentCommands[3];
    unsigned int nextSequence = 1;

    int myPlayerIndex = -1;

    std::deque<TimedSnapshot> snapshotBuffer;
};
#pragma once
#include <glm/glm.hpp>

struct PlayerSnapshot {
    glm::vec3 position;
    glm::vec3 lookDirection;
    float health;
    bool sliding;
    float dashTimeLeft;
};

struct Snapshot {
    unsigned int tick; //for reconciliation later on
    int playerCount;
    PlayerSnapshot players[2]; //fixed size so it can go in a net packet
    int roundWins[2];
    int phase; //RoundPhase as an int
};

enum class MessageType : unsigned char { //bytes that every packet starts with to identify its type
    Snapshot = 0,
    Welcome = 1
};

struct WelcomeMessage { //when a client connects
    int playerIndex;
};
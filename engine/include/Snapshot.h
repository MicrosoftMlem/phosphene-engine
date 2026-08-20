#pragma once
#include "InputCommand.h"
#include <glm/glm.hpp>

struct PlayerSnapshot {
  unsigned int lastAppliedSequence;

  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 lookDirection;
  glm::vec3 dashDirection;
  float health;
  float dashTimeLeft;
  float dashRechargeTimer;
  int dashCharges;
  bool grounded;
  bool sliding;
};

struct EntitySnapshot {
  int type; // EntityType as an int
  glm::vec3 position;
  float rotationY;
  bool isRed;
  bool isAmber;
};

struct Snapshot {
  unsigned int tick; // for reconciliation
  int playerCount;
  PlayerSnapshot players[2]; // fixed size so it can go in a net packet
  int roundWins[2];
  int phase; // RoundPhase as an int
  int entityCount;
  EntitySnapshot entities[16]; // currently 16 max.
};

enum class MessageType : unsigned char { // bytes that every packet starts with
                                         // to identify its type
  Snapshot = 0,
  Welcome = 1
};

struct WelcomeMessage { // when a client connects
  int playerIndex;
};

struct CommandPacket {
  InputCommand commands[3]; // the last 3 commands, newest last
};

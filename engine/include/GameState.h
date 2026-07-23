#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include "AABB.h"
#include "WorldEntity.h"

class Item; //forward declare
class WorldEntity; //forward declare

enum class EquipSlot { Weapon, Ability };

enum class RoundPhase { Active, RoundOver, MatchOver };

struct PlayerState {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 lookDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    float health = 100.0f;

    float baseMoveSpeed = 8.0f;
    float baseJumpStrength = 8.0f;
    float baseGravity = -20.0f;
    float baseGroundAccel = 12.0f;
    float baseAirAccel = 2.0f;

    float moveSpeed = 8.0f;
    float jumpStrength = 8.0f;
    float gravity = -20.0f;
    float groundAccel = 12.0f;
    float airAccel = 2.0f;
    bool grounded = false;
    bool frozen = false;

    int dashCharges = 3;
    float dashRechargeTimer = 0.0f;
    float dashTimeLeft = 0.0f; //how long the current dash/burst lasts
    glm::vec3 dashDirection = glm::vec3(0.0f);

    bool sliding = false;

    Item* weapon = nullptr;
    Item* ability = nullptr;
    EquipSlot equipped = EquipSlot::Weapon;
};

struct GameState {
    std::vector<PlayerState> players;
    std::vector<AABB> colliders;
    std::vector<WorldEntity*> worldEntities;

    RoundPhase phase = RoundPhase::Active;
    int roundWins[2] = {0, 0};
    float phaseTimer = 0.0f;
    int matchWinner = -1; 
};


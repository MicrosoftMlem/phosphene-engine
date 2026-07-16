#pragma once
#include <vector>
#include <glm/glm.hpp>

class Item; //forward declare

enum class EquipSlot { Weapon, Ability };

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

    Item* weapon = nullptr;
    Item* ability = nullptr;
    EquipSlot equipped = EquipSlot::Weapon;
};

struct GameState {
    std::vector<PlayerState> players;
};
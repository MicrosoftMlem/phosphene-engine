#pragma once
#include <vector>
#include <glm/glm.hpp>

class Item; //forward declare

enum class EquipSlot { Weapon, Ability };

struct PlayerState {
    glm::vec3 position;
    float health = 100.0f;
    Item* weapon = nullptr;
    Item* ability = nullptr;
    EquipSlot equipped = EquipSlot::Weapon;
};

struct GameState {
    std::vector<PlayerState> players;
};
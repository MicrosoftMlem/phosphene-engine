#pragma once
#include <glm/glm.hpp>
#include "Material.h"

struct GameState; //forward declare

enum class EntityType {
    TrafficLight
};

class WorldEntity {
public:
    virtual ~WorldEntity() {} //destructor
    virtual void update(GameState& state, float deltaTime) = 0; 
    virtual bool isExpired() const { return false; }
    virtual EntityType getType() const = 0;

    glm::vec3 position;
    int ownerIndex = -1;
    Material material;
};
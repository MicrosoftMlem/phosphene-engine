#pragma once
#include <glm/glm.hpp>

struct GameState; //forward declare

class WorldEntity {
public:
    virtual ~WorldEntity() {} //destructor
    virtual void update(GameState& state, float deltaTime) = 0; 
    virtual bool isExpired() const { return false; }

    glm::vec3 position;
    int ownerIndex = -1;
};
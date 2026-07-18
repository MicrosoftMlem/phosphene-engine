#include "TrafficLightEntity.h"
#include "GameState.h"
#include "Utils.h"
#include <iostream>


TrafficLightEntity::TrafficLightEntity(glm::vec3 pos, int owner) {
    position = pos; //these vars ('position' and 'ownerIndex') are inherited from WorldEntity
    ownerIndex = owner;
    lifetime = 22.0f; // 22 seconds
}


void TrafficLightEntity::update(GameState& state, float deltaTime) {
    timer += deltaTime;
    lifetime -= deltaTime;


    if (timer >= 5.5f) {
        isRed = !isRed; // toggle the light color
        timer = 0.0f; // reset the timer
    }

    if (isRed) {
        for (PlayerState& player : state.players) {
            glm::vec3 toLight = glm::normalize(position - player.position);
            if (glm::dot(player.lookDirection, toLight) > 0.4f) {
                glm::vec3 eyePos = player.position + glm::vec3(0.0f, 1.7f, 0.0f);

                if (hasLineOfSight(eyePos, position, state.colliders)) {
                    std::cout << "Traffic light is red, and player is frozen.\n";
                    player.frozen = true;
                }
            }
            
        }
    }
}

bool TrafficLightEntity::isExpired() const {
    if (lifetime <= 0.0f) {
        return true;
    }
    return false;
}


EntityType TrafficLightEntity::getType() const {
    return EntityType::TrafficLight;
}
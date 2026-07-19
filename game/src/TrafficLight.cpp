#include "TrafficLight.h"
#include "GameState.h"
#include "TrafficLightEntity.h"
#include <glm/glm.hpp>
#include <iostream>

void TrafficLight::use(GameState& state, int holderIndex, UseType type) {
    if (type == UseType::Primary && charges > 0) {
        std::cout << "Traffic light used by player " << holderIndex << "\n";
        charges--;
        glm::vec3 spawnPos = state.players[holderIndex].position + glm::vec3(0.0f, 0.7f, 0.0f); //spawn at chest height bc we're throwing it
        TrafficLightEntity* e = new TrafficLightEntity(spawnPos, holderIndex);
        e->throwFrom(state.players[holderIndex].lookDirection, 8.0f, 10.0f); 
        state.worldEntities.push_back(e);
    }
}


#include "TrafficLight.h"
#include "GameState.h"
#include "TrafficLightEntity.h"
#include <glm/glm.hpp>
#include <iostream>

void TrafficLight::use(GameState& state, int holderIndex, UseType type) {
    if (type == UseType::Primary && charges > 0) {
        std::cout << "Traffic light used by player " << holderIndex << "\n";
        charges--;
        glm::vec3 spawnPos = state.players[holderIndex].position + glm::vec3(0.0f, 0.5f, 0.0f); //needs to be made nicer (slide along floor etc)
        state.worldEntities.push_back(new TrafficLightEntity(spawnPos, holderIndex));
    }
}


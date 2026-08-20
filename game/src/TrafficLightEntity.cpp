#include "TrafficLightEntity.h"
#include "GameState.h"
#include "Simulation.h"
#include "Utils.h"
#include <iostream>

TrafficLightEntity::TrafficLightEntity(glm::vec3 pos, int owner) {
  position = pos; // these vars ('position' and 'ownerIndex') are inherited from
                  // WorldEntity
  ownerIndex = owner;
  lifetime = 22.0f; // 22 seconds
}

void TrafficLightEntity::update(GameState &state, float deltaTime) {
  // physics pass:

  velocity.y += -20 * deltaTime; // gravity

  // collision size moved into world entity;

  // this does all the actual physics
  bool grounded =
      resolveAABBCollision(position, velocity, collisionBoxMin, collisionBoxMax,
                           state.colliders, deltaTime);

  if (grounded) {
    velocity.x *= 0.9f;
    velocity.z *= 0.9;
    spinSpeed *= 0.9;

    if (glm::length(glm::vec3(velocity.x, 0.0f, velocity.z)) <
        0.1f) { // if we have basically stopped
      velocity.x = 0.0f;
      velocity.z = 0.0f;
      spinSpeed = 0.0f;
      settled = true;
    }
  }

  rotationY += spinSpeed * deltaTime;

  // timer stuff:
  timer += deltaTime;
  lifetime -= deltaTime;

  isAmber = false;
  if (!isRed) {
    if (timer > 4.8f) {
      isAmber = true;
    } else {
      isAmber = false;
    }
  } else {
    isAmber = false;
  }

  if (timer >= 5.5f) {
    isRed = !isRed; // toggle the light color
    isAmber = false;
    timer = 0.0f; // reset the timer
  }

  if (isRed) {
    for (PlayerState &player : state.players) {
      glm::vec3 toLight = glm::normalize(position - player.position);
      if (glm::dot(player.lookDirection, toLight) > 0.4f) {
        glm::vec3 eyePos = player.position + glm::vec3(0.0f, 1.7f, 0.0f);

        if (hasLineOfSight(eyePos, position, state.colliders)) {
          // std::cout << "Traffic light is red, and player is frozen.\n";
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

void TrafficLightEntity::throwFrom(glm::vec3 direction, float speed,
                                   float spin) {
  velocity = direction * speed;
  spinSpeed = spin;
}

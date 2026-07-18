#pragma once
#include "WorldEntity.h"

class TrafficLightEntity : public WorldEntity {
public:
    TrafficLightEntity(glm::vec3 pos, int owner);
    void update(GameState& state, float deltaTime) override;
    bool isExpired() const override;
private:
    float timer = 0.0f;
    bool isRed = false;
    float lifetime = 0.0f;
};
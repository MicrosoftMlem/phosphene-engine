#pragma once
#include "WorldEntity.h"

class TrafficLightEntity : public WorldEntity {
public:
    TrafficLightEntity(glm::vec3 pos, int owner);
    void update(GameState& state, float deltaTime) override;
    bool isExpired() const override;
    virtual EntityType getType() const override;
    bool getIsRed() const { return isRed; }
    float getRotationY() const { return rotationY; }

    void throwFrom(glm::vec3 direction, float speed, float spin);
    
private:
    float timer = 0.0f;
    bool isRed = false;
    float lifetime = 0.0f;

    glm::vec3 velocity = glm::vec3(0.0f);
    float rotationY = 0.0f; //cosmetic spin angle in radians
    float spinSpeed = 0.0f; //decays over time
    bool settled = false;
};
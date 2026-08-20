#pragma once
#include "WorldEntity.h"

class TrafficLightEntity : public WorldEntity {
public:
  TrafficLightEntity(glm::vec3 pos, int owner);
  void update(GameState &state, float deltaTime) override;
  bool isExpired() const override;
  virtual EntityType getType() const override;
  bool getIsRed() const { return isRed; }
  bool getIsAmber() const { return isAmber; }
  float getRotationY() const { return rotationY; }

  void throwFrom(glm::vec3 direction, float speed, float spin);

private:
  glm::vec3 collisionBoxMin = glm::vec3(-0.3f, 0.0f, -0.3f);
  glm::vec3 collisionBoxMax = glm::vec3(0.3f, 1.0f, 0.3f);

  float timer = 0.0f;
  bool isRed = false;
  bool isAmber = false;
  float lifetime = 0.0f;

  glm::vec3 velocity = glm::vec3(0.0f);
  float rotationY = 0.0f; // cosmetic spin angle in radians
  float spinSpeed = 0.0f; // decays over time
  bool settled = false;
};

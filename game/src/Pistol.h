#pragma once
#include "Item.h"
#include "GameState.h"

class Pistol : public Item {
public:
  void use(GameState& state, int holderIndex, UseType type) override; //say that this declaration overrides Item classes one
  int ammo = 18;
  int damage = 12;

private:
  virtual void tickInternal(float deltaTime);

  float primaryCooldown = 0.0f;
  float primaryRate = 4.0f;

  float secondaryCooldown = 0.0f;
  float secondaryRate = 25.0f;
};


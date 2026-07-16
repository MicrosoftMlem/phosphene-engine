#pragma once
#include "Item.h"
#include "GameState.h"

class ButterflyKnife : public Item {
public:
    void use(GameState& state, int holderIndex, UseType type) override;
    void passiveUpdate(GameState& state, int holderIndex, float deltaTime) override;
};
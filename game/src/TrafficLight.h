#pragma once
#include "Item.h"
#include "GameState.h"

class TrafficLight : public Item {
public:
    void use(GameState& state, int holderIndex, UseType type) override; //say that this declaration overrides Item classes one

private:
    int charges = 1;
};


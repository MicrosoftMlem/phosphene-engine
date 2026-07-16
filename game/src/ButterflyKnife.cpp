#include "ButterflyKnife.h"
#include "GameState.h"
#include <iostream>

void ButterflyKnife::passiveUpdate(GameState& state, int holderIndex, float deltaTime) {
    PlayerState& player = state.players[holderIndex]; //& bc otherwise player is a copy. we need a reference (&)

    player.moveSpeed *= 1.75f; //multiply so multiple effects can stack
    player.airAccel *= 2.0f;
    player.groundAccel *= 1.25f;
    player.gravity *= 0.75f;
    player.jumpStrength *= 0.8f;
}

void ButterflyKnife::use(GameState& state, int holderIndex, UseType type) {
    if (type == UseType::Primary) {
        std::cout << "Used Butterfly Knife primary.\n"; 
    }
}
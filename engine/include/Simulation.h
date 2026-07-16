#pragma once
#include "GameState.h"
#include "InputCommand.h"

void simulate(GameState& state, int playerIndex, const InputCommand& command, float deltaTime);

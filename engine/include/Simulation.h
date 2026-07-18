#pragma once
#include "GameState.h"
#include "InputCommand.h"
#include "AABB.h"
#include <vector>

void simulate(GameState& state, int playerIndex, const InputCommand& command, float deltaTime);

#pragma once
#include "GameState.h"
#include "InputCommand.h"

InputCommand computeBotCommand(const GameState& state, int botIndex);
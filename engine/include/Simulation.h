#pragma once
#include "GameState.h"
#include "InputCommand.h"
#include "AABB.h"
#include <vector>

void processPlayerInput(GameState& state, int playerIndex, const InputCommand& command, float deltaTime);
void updateWorld(GameState& state, const std::vector<glm::vec3>& spawns, float deltaTime);
void resetPlayerStats(PlayerState& player);
bool resolveAABBCollision(glm::vec3& position, glm::vec3& velocity, glm::vec3 boxMin, glm::vec3 boxMax, const std::vector<AABB>& colliders, float deltaTime);
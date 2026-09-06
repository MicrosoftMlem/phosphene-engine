#pragma once

#include "miniaudio/miniaudio.h"
#include "AudioInstance.h"
#include "GameState.h"
#include <vector>
#include <glm/glm.hpp>

class FootstepSystem {
public:
  FootstepSystem();
  void update(std::vector<PlayerState> players, float deltaTime);

private:
  std::vector<ma_sound> footstepSounds;
  // indexed by player index
  std::vector<glm::vec3> playerPositions;
};

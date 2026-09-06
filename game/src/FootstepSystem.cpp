#include "FootstepSystem.h"
#include "miniaudio/miniaudio.h"
#include "AudioInstance.h"
#include "GameState.h"
#include <glm/geometric.hpp>
#include <vector>
#include <iostream>

//rn sound paths and the vector indexes are hardcoded so could cause issues if not noticed.

FootstepSystem::FootstepSystem() {
  AudioInstance audio;
  
  ma_result result;
  result = ma_sound_init_from_file(&audio.engine, "footstep01.mp3", MA_SOUND_FLAG_DECODE,
                          NULL, NULL, &footstepSounds[0]);

  if (result != MA_SUCCESS) {
    std::cerr << "Failed to add footstep01.mp3 to FootstepSystem.\n";
  }

  result = ma_sound_init_from_file(&audio.engine, "footstep02.mp3",
                                   MA_SOUND_FLAG_DECODE, NULL, NULL,
                                   &footstepSounds[1]);

  if (result != MA_SUCCESS) {
    std::cerr << "Failed to add footstep02.mp3 to FootstepSystem.\n";
  }
}

void FootstepSystem::update(std::vector<PlayerState> players, float deltaTime) {
  // doesnt validate if any of the entries of the footstepSounds vector are
  // invalid.

  for (int playerIndex = 0; playerIndex < players.size(); playerIndex++) {
    glm::vec3 oldPos = playerPositions[playerIndex];
    glm::vec3 newPos = players[playerIndex].position;

    glm::vec2 old2D = glm::vec2(oldPos.x, oldPos.z);
    glm::vec2 new2D = glm::vec2(newPos.x, newPos.z);
    
    float distance = glm::distance(old2D, new2D);
    playerPositions[playerIndex] = newPos;

    if (distance <= 1.0f) {
      for (ma_sound sound : footstepSounds) {
        if (!ma_sound_is_playing(&sound)) {
          ma_sound_seek_to_pcm_frame(&sound, 0);
          // we dont do pos yet as currently we dont consider player pos TODO
          // ISSUE ma_sound_set_position(&s, x, y, z);

          ma_sound_set_pitch(&sound, 1.0f);
          ma_sound_set_volume(&sound, 1.0f);

          ma_sound_start(&sound);
        }
      }
    }
  }
}

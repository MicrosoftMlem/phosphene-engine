#include "AudioInstance.h"
#include "miniaudio/miniaudio.h"
#include <iostream>
#include <string>

AudioInstance::AudioInstance() {
  ma_result result;

  result = ma_engine_init(NULL, &this->engine);

  if (result != MA_SUCCESS) {
    std::cerr << "AudioInstance had engine init not successful.\n";
    return;
  }

  std::cout << "AudioEngine init successfully.\n";
}

int AudioInstance::playAudio(std::string path) {
  // assumes engine is initialised successfully

  ma_result result = ma_engine_play_sound(&engine, path.c_str(), NULL);

  if (result != MA_SUCCESS) {
    std::cerr << "Failed to play sound at: " << path << "\n";
    return -1;
  }

  return 0;
}

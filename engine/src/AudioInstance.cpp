#include "AudioInstance.h"
#include "miniaudio/miniaudio.h"
#include <iostream>
#include <string>

// this class handles construction/destruction only rn (doesnt have desctruction yet)

AudioInstance::AudioInstance() {
  ma_result result;

  result = ma_engine_init(NULL, &this->engine);

  if (result != MA_SUCCESS) {
    std::cerr << "AudioInstance had engine init not successful.\n";
    return;
  }

  std::cout << "AudioEngine init successfully.\n";
}


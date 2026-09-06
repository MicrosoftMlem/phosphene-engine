#pragma once

#include "miniaudio/miniaudio.h"
#include <string>

class AudioInstance {
public:
  AudioInstance();
  int playAudio(std::string path);

    private :
  ma_engine engine;
};

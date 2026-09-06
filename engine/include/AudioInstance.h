#pragma once

#include "miniaudio/miniaudio.h"
#include <string>

class AudioInstance {
public:
  AudioInstance();

  ma_engine engine;
};

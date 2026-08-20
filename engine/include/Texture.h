#pragma once
#include "glad/glad.h"

class Texture {

public:
  unsigned int ID;

  Texture(const char *path);
  void bind(int slot = 0); // active the texture before drawing
};

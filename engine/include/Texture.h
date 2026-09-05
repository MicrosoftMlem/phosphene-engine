#pragma once
#include "glad/glad.h"

class Texture {

public:
  unsigned int ID;

  int width;
  int height;
  
  Texture(const char *path);
  void bind(int slot = 0); // active the texture before drawing
};

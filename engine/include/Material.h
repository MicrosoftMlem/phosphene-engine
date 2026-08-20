#pragma once
#include "Texture.h"
#include <glm/glm.hpp>

struct Material {
  Texture *texture = nullptr;
  glm::vec3 tint = glm::vec3(1.0f);
  glm::vec2 textureScale = glm::vec2(1.0f);
};

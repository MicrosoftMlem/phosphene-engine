#pragma once
#include "Texture.h"
#include <glm/glm.hpp>
#include <string>
#include "Shader.h"

struct Material {
  Texture *texture = nullptr;
  glm::vec3 tint = glm::vec3(1.0f);
  glm::vec2 textureScale = glm::vec2(1.0f);
  std::string vertShaderPath = "basic.vert";
  std::string fragShaderPath = "basic.frag";
  Shader *shader = nullptr;

  // texture and shader are pointers so that we can have
  // multiple Materials share same shader
};

#pragma once
#include <glm/glm.hpp>
#include "Texture.h"

struct Material {
    Texture* texture = nullptr;
    glm::vec3 tint = glm::vec3(1.0f);
    glm::vec3 emissive = glm::vec3(0.0f);
    glm::vec2 textureScale = glm::vec2(1.0f);
};
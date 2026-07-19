#pragma once

#include <glm/glm.hpp>
#include "Shader.h"

class UIRenderer {
public:
    UIRenderer();
    void drawRect(float x, float y, float width, float height, glm::vec4 color, int screenWidth, int screenHeight);

private:
    Shader shader;
    unsigned int quadVAO;
    unsigned int quadVBO;
};
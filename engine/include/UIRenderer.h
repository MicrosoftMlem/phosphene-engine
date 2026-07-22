#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"

class UIRenderer {
public:
    UIRenderer();
    void drawRect(float x, float y, float width, float height, glm::vec4 color, int screenWidth, int screenHeight);
    void drawFireRect(float x, float y, float width, float height, float time, float pixelSize, glm::vec3 color1, glm::vec3 color2, int screenWidth, int screenHeight);
    void drawCircle(float x, float y, float diameter, glm::vec4 color, int screenWidth, int screenHeight);

private:
    Shader shader;
    Shader fireShader;
    Shader circleShader;
    unsigned int quadVAO;
    unsigned int quadVBO;
};
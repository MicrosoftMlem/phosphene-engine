#pragma once

#include <string>
#include "Texture.h"

void initTextures();
void markTextureDeprecated(const std::string &stringName);
Texture *getTexture(const std::string &textureName);


#pragma once

#include <string>
#include "Shader.h"

void initShaders();
Shader *getShader(const std::string &shaderName);

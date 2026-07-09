#pragma once
#include <glad/glad.h>

class Shader {
public:
    unsigned int ID;
    
    Shader(const char* vertPath, const char* fragPath);
    void use();
};
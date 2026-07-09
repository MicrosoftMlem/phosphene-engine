#pragma once
#include "glad/glad.h"

class Texture {

public:
    unsigned int ID;

    Texture(const char* path);
    void bind(); //active the texture before drawing

};
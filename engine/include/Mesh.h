#pragma once
#include <glad/glad.h>
#include <vector>

class Mesh {
public:
    Mesh(std::vector<float> vertices);
    void draw();
    

private:
    unsigned int VAO, VBO;
    int vertexCount;
};

std::vector<float> computeNormals(const std::vector<float>& verts);
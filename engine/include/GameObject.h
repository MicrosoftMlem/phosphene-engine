#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "AABB.h"
#include "Texture.h"

class GameObject {
public:
    glm::vec3 position;
    glm::vec3 size; //width/height/depth
    Mesh* mesh; //pointer bc then gameobjects of same mesh can share for optimisation
    Texture* texture;
    glm::vec2 textureScale;

    GameObject(Mesh* mesh, Texture* texture, glm::vec3 position, glm::vec3 size, glm::vec2 textureScale);

    AABB getAABB();
    void draw(Shader& shader);
};
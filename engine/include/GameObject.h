#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "AABB.h"
#include "Texture.h"
#include "Material.h"

class GameObject {
public:
  glm::vec3 position;
  glm::vec3 size; //width/height/depth
  Mesh* mesh; //pointer bc then gameobjects of same mesh can share for optimisation
  Material material;

  bool collidable = true;

  GameObject(Mesh* mesh, Material material, glm::vec3 position, glm::vec3 size);

  AABB getAABB();
  void draw();
  //    void draw(Shader& shader);
};

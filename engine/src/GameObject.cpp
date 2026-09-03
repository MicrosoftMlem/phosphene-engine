#include "GameObject.h"
#include "AABB.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

GameObject::GameObject(Mesh *mesh, Material material, glm::vec3 position,
                       glm::vec3 size) {
  this->mesh = mesh; // bc mesh is an arg but also we have the member 'mesh' we
                     // use -> to specify its our member
  this->position = position;
  this->size = size;
  this->material = material;

  if (this->material.shader == nullptr) {
    std::cerr << "Material shader is null for GameObject\n";
  }
}

AABB GameObject::getAABB() {
  AABB box;
  glm::vec3 half = size * 0.5f;
  box.min = position - half;
  box.max = position + half;
  return box;
}

void GameObject::draw() {
  Shader *matShader = material.shader;
  // shader.use();
  matShader->use();
  

  glm::mat4 model = glm::mat4(1.0f);       // identity
  model = glm::translate(model, position); // move it to its position
  model = glm::scale(model, size);         // scale it to its size

  int modelLoc =
      glGetUniformLocation(matShader->ID, "model"); // get the shader model uniform
  glUniformMatrix4fv(
      modelLoc, 1, GL_FALSE,
      glm::value_ptr(model)); // store the model matrix in the uniform

  if (material.texture) {
    material.texture->bind(0); // bind the texture
  }

  int tintLoc = glGetUniformLocation(matShader->ID, "tint");
  glUniform3f(tintLoc, material.tint.r, material.tint.g, material.tint.b);

  int scaleLoc = glGetUniformLocation(
      matShader->ID, "textureScale"); // get the texture scale uniform
  glUniform2f(scaleLoc, material.textureScale.x,
              material.textureScale.y); // set it to the materials textureScale

  glUniform1i(glGetUniformLocation(matShader->ID, "u_UseEmissive"), 0); // 0 = false

  glUniform1i(glGetUniformLocation(matShader->ID, "u_AlbedoMap"), 0);
  // point it to slot 0

  mesh->draw(); // tell the mesh to draw '->' for same reason as comment at top
}

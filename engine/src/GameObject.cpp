#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GameObject.h"
#include "AABB.h"

GameObject::GameObject(Mesh* mesh, Material material, glm::vec3 position, glm::vec3 size) {
    this->mesh = mesh; //bc mesh is an arg but also we have the member 'mesh' we use -> to specify its our member
    this->position = position;
    this->size = size;
    this->material = material;

}

AABB GameObject::getAABB() {
    AABB box;
    glm::vec3 half = size * 0.5f;
    box.min = position - half;
    box.max = position + half;
    return box;
}

void GameObject::draw(Shader& shader) {
    glm::mat4 model = glm::mat4(1.0f); //identity
    model = glm::translate(model, position); //move it to its position
    model = glm::scale(model, size); //scale it to its size

    int modelLoc = glGetUniformLocation(shader.ID, "model"); //get the shader model uniform
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //store the model matrix in the uniform

    if (material.texture) {
        material.texture->bind(); //bind the texture
    }

    int tintLoc = glGetUniformLocation(shader.ID, "tint");
    glUniform3f(tintLoc, material.tint.r, material.tint.g, material.tint.b);

    int emissiveLoc = glGetUniformLocation(shader.ID, "emissive");
    glUniform3f(emissiveLoc, material.emissive.r, material.emissive.g, material.emissive.b);

    int scaleLoc = glGetUniformLocation(shader.ID, "textureScale"); //get the texture scale uniform
    glUniform2f(scaleLoc, material.textureScale.x, material.textureScale.y); //set it to the materials textureScale

    mesh->draw(); //tell the mesh to draw '->' for same reason as comment at top
}
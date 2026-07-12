#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "Camera.h"

class Player {
public:
    glm::vec3 position;
    glm::vec3 velocity; //how fast i am moving per axis in a second

    float moveSpeed;
    float gravity;

    Player(glm::vec3 startPos);

    void update(GLFWwindow* window, Camera& camera, float deltaTime);

private:
    void handleInput(GLFWwindow* window, Camera& camera, float deltaTime);
    void applyPhysics(float deltaTime);
};
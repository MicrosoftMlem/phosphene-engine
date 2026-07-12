#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "AABB.h"

class Player {
public:
    glm::vec3 position;
    glm::vec3 velocity; //how fast i am moving per axis in a second

    float moveSpeed;
    float gravity;
    float jumpStrength;
    float groundAccel;
    float airAccel;
    float eyeHeight;

    Player(glm::vec3 startPos);

    void update(GLFWwindow* window, Camera& camera, const std::vector<AABB>& colliders, float deltaTime);

    AABB getAABB();

private:
    void handleInput(GLFWwindow* window, Camera& camera, float deltaTime);
    void resolveCollisions(const std::vector<AABB>& colliders, float deltaTime);
    bool grounded;
};
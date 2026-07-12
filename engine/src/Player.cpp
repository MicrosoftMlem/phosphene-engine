#include "Player.h"

Player::Player(glm::vec3 startPos) {
    position = startPos;
    velocity = glm::vec3(0.0f);
    moveSpeed = 5.0f;
    gravity = -20.0f;
    jumpStrength = 8.0f;
    grounded = false;
}

void Player::update(GLFWwindow* window, Camera& camera, float deltaTime) {
    handleInput(window, camera, deltaTime);
    applyPhysics(deltaTime);

    if (position.y < 0.0f) { //temporary simulation of a floor
        position.y = 0.0f;
        velocity.y = 0.0f;
        grounded = true;
    }
    else {
        grounded = false;
    }

    camera.position = position; // camera follows player
}


void Player::handleInput(GLFWwindow* window, Camera& camera, float deltaTime) {
    glm::vec3 flatFront = camera.front; //flatfront is forward dir WITHOUT up/down tilt
    flatFront.y = 0.0f; //remove like up/down tilt
    flatFront = glm::normalize(flatFront);
    glm::vec3 flatRight = glm::normalize(glm::cross(flatFront, glm::vec3(0, 1, 0))); //get flatRight from cross product

    glm::vec3 moveDir = glm::vec3(0.0f); //a vector of our movement input (will be normalized)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){ //polls if W is held down
        moveDir += flatFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        moveDir -= flatFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        moveDir -= flatRight;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        moveDir += flatRight;
    }

    if (glm::length(moveDir) > 0.0f) { //if there is movement input
        moveDir = glm::normalize(moveDir);
    }
    velocity.x = moveDir.x * moveSpeed;
    velocity.z = moveDir.z * moveSpeed;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && grounded) { //jump
        velocity.y = jumpStrength;
    }
}


void Player::applyPhysics(float deltaTime) {
    velocity.y += gravity * deltaTime;
    position += velocity * deltaTime;
}
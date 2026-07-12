#include "Player.h"

Player::Player(glm::vec3 startPos) {
    position = startPos;
    velocity = glm::vec3(0.0f);
    moveSpeed = 5.0f;
    gravity = -20.0f;

}

void Player::update(GLFWwindow* window, Camera& camera, float deltaTime) {
    handleInput(window, camera, deltaTime);
    camera.position = position; // camera follows player
}

void Player::handleInput(GLFWwindow* window, Camera& camera, float deltaTime) {
    glm::vec3 flatFront = camera.front; //flatfront is forward dir WITHOUT up/down tilt
    flatFront.y = 0.0f; //remove like up/down tilt
    flatFront = glm::normalize(flatFront);

    glm::vec3 flatRight = glm::normalize(glm::cross(flatFront, glm::vec3(0, 1, 0))); //get flatRight from cross product


    float deltaMoveSpeed = moveSpeed * deltaTime;  
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){ //polls if W is held down
        position += deltaMoveSpeed * flatFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        position -= deltaMoveSpeed * flatFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        position -= flatRight * deltaMoveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        position += flatRight * deltaMoveSpeed;
    }
}
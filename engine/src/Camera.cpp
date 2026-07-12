#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"



Camera::Camera(glm::vec3 startPos) {

    position = startPos;
    front = glm::vec3(0, 0, -1);
    up = glm::vec3(0, 1, 0);
    yaw = -90.0f;
    pitch = 0.0f;
    speed = 5.0f;
    sensitivity = 0.1f;

}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(GLFWwindow* window, float deltaTime) {
    float deltaSpeed = speed * deltaTime;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){ //polls if W is held down
        position += deltaSpeed * front;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        position -= deltaSpeed * front;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        position -= glm::normalize(glm::cross(front, up)) * deltaSpeed; //the direction perpedicular to front and up = right (and left if negative)
                                                                                // we normalise it bc otherwise strafe speed would change based on angles 
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        position += glm::normalize(glm::cross(front, up)) * deltaSpeed;
    }
}

void Camera::processMouse(float xoffset, float yoffset) {
    //'firstpos' checks are done by the caller (i think) - mouse_callback(...)

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0) {
        pitch = 89.0;
    }
    else if (pitch < -89.0) {
        pitch = -89.0;
    }

    updateVectors();
}

void Camera::updateVectors() {
    //this is trig and idk how it works
    //it gets front by using yaw and pitch etc then  normalises it bc front is a direction
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
}

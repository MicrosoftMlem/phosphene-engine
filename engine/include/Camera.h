#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;

    float yaw;
    float pitch;
    float speed;
    float sensitivity;

    Camera(glm::vec3 startPos);

    glm::mat4 getViewMatrix();
    void processKeyboard(GLFWwindow* window, float deltaTime);
    void processMouse(float xoffset, float yoffset);

private:
    void updateVectors();

};
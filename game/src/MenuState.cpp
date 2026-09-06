#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "MenuState.h"
#include "StateManager.h"
#include "UIRenderer.h"
#include "textureStore.h"

MenuState::MenuState(GLFWwindow *window) {
  this->window = window;

  testTex = *getTexture(testTextureStringName);
}

void MenuState::update(float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        manager->requestMatch("127.0.0.1", 7777);
    }
}

void MenuState::render() {
    glClearColor(0.1f, 0.05f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
  
    uiRenderer.drawImage(40, 40, 1, 1, testTex, w, h);
}

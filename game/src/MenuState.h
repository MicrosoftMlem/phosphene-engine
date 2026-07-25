#pragma once
#include "GameStateBase.h"


struct GLFWwindow; //forward declare

class MenuState : public GameStateBase {
public:
    MenuState(GLFWwindow* window);
    void update(float deltaTime) override;
    void render() override;

private:
    GLFWwindow* window;
};
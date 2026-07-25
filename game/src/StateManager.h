#pragma once
#include "GameStateBase.h"
#include "NetworkClient.h"


struct GLFWwindow; //forward declare. we dont need to pull in all of <GLFW/glfw3.h> just for this singular GLFWwindow

class StateManager {
public:
    StateManager(GLFWwindow* window);
    ~StateManager();

    void update(float deltaTime);
    void render();

    void requestMenu();
    void requestMatch(const char* host, int port);

    GLFWwindow* getWindow() { return window; }
    NetworkClient& getNetwork() { return network; }

private:
    void performTransition();
    GLFWwindow* window;
    NetworkClient network; //owned here so it survives state swaps
    GameStateBase* currentState;
    enum class Pending { None, Menu, Match };
    Pending pending = Pending::None;
    const char* pendingHost = nullptr;
    int pendingPort = 0;
};
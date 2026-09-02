#include "StateManager.h"
#include "PlayingState.h"
#include "MenuState.h"


StateManager::StateManager(GLFWwindow* window) {
    this->window = window;
    currentState = nullptr;
}

StateManager::~StateManager() {
    delete currentState;
}


void StateManager::update(float deltaTime) {
    if (currentState) {
        currentState->update(deltaTime);
    }
    performTransition(); //always transition after update so we dont do it in the middle of smth
}

void StateManager::render() {
    if (currentState) {
        currentState->render();
    }
}

void StateManager::requestMenu() {
    pending = Pending::Menu;
}

void StateManager::requestMatch(const char* host, int port) {
    pending = Pending::Match;
    pendingHost = host;
    pendingPort = port;
}

// Transitions between states. Run in StateManager::update
void StateManager::performTransition() {
    if (pending == Pending::None) return; //no action to be done

    delete currentState; //we are after update so we can delete it
    currentState = nullptr;

    if (pending == Pending::Match) {
        network.connectToServer(pendingHost, pendingPort);
        currentState = new PlayingState(window, network);
    }
    else {
        currentState = new MenuState(window);
    }
    currentState->manager = this;

    pending = Pending::None;
}

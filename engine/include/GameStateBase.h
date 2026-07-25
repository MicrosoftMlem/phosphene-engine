#pragma once


class StateManager; //forward declare

//actual game state machine

class GameStateBase {
public:
    virtual ~GameStateBase() {} //destructor
    virtual void update(float deltaTime) = 0; //= 0 means it has to be implemented
    virtual void render() = 0;

    StateManager* manager = nullptr; //set by the manager when the state is created
};
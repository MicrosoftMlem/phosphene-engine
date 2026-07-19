#pragma once

//theres no cpp file for this? im not sure why i made it

//actual game state machine

class GameStateBase {
public:
    virtual ~GameStateBase() {} //destructor
    virtual void update(float deltaTime) = 0; //= 0 means it has to be implemented
    virtual void render() = 0;
};
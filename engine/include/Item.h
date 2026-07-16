#pragma once

struct GameState; //forward declaraion so we can get references to it without including it. bc if gamestate included item.h and does stuff
//there could be a circular include
struct PlayerState;

enum class UseType {
    Primary,
    Secondary
};


class Item {
public:
    virtual ~Item() {} //virtual destructor, has to have it
    virtual void use(GameState& state, int holderIndex, UseType type) = 0; //fully virtual, items have to implement
    virtual void passiveUpdate(GameState& state, int holderIndex, float deltaTime) {} //virtual with empty default (doesnt have to be implemented)

    virtual void tickInternal(float deltaTime) {} //for stuff like cooldown timers
    virtual bool isAlwaysActive() const { return false; } //to query if passiveUpdate is always on. by default just return false
};

#include "Simulation.h"
#include "Item.h"
#include "AABB.h"
#include "Utils.h"
#include "WorldEntity.h"
#include <vector>
#include <algorithm>
#include <iostream>

//simulate is the server-side code

//simulate() cant see the funcs below it in the file so we forward-declare them here:
//an alternative would be to move these funcs to the top
static AABB getPlayerAABB(const PlayerState& player);
static void applyMovementInput(PlayerState& player, const InputCommand& command, float deltaTime);
static void resolvePlayerCollisions(PlayerState& player, const std::vector<AABB>& colliders, float deltaTime);


void simulate(GameState& state, int playerIndex, const InputCommand& command, float deltaTime) {
    //std::cout << "deltaTime is: " << deltaTime << std::endl; //endl bc i cba doing newline
    
    PlayerState& player = state.players[playerIndex]; //our players PlayerState

    player.moveSpeed = player.baseMoveSpeed; //reset the stats (so items can update them below, or if nothing changes them they're back at defaults)
    player.jumpStrength = player.baseJumpStrength;
    player.gravity = player.baseGravity;
    player.groundAccel = player.baseGroundAccel;
    player.airAccel = player.baseAirAccel;
    player.lookDirection = command.lookDirection;
    player.frozen = false;


    if (command.equipWeapon) { //weapons first incase they modify movement (below)
        player.equipped = EquipSlot::Weapon;
    }
    if (command.equipAbility) {
        player.equipped = EquipSlot::Ability;
    }

    Item* held = (player.equipped == EquipSlot::Weapon) ? player.weapon : player.ability;

    if (player.weapon) {
        player.weapon->tickInternal(deltaTime);
    }
    if (player.ability) {
        player.ability->tickInternal(deltaTime);
    }

    if (player.weapon && (player.weapon == held || player.weapon->isAlwaysActive())) {
        //if we have a weapon, AND we are either holding it or it always ticks, tick it
        player.weapon->passiveUpdate(state, playerIndex, deltaTime);
    }

    if (player.ability && (player.ability == held || player.ability->isAlwaysActive())) {
        //if we have an ability, AND we are either holding it or it always ticks, tick it
        player.ability->passiveUpdate(state, playerIndex, deltaTime);
    }

    if (held) {
        
        if (command.primaryPressed) { //currently hardcoded that left click is semi auto 
            held->use(state, playerIndex, UseType::Primary);
        }
        if (command.secondaryHeld) {//and right click is auto
            held->use(state, playerIndex, UseType::Secondary);
        }
    }

    for (WorldEntity* entity : state.worldEntities) {
        entity->update(state, deltaTime);
    }

    for (WorldEntity*& entity : state.worldEntities) {
        if (entity->isExpired()) {
            delete entity; //free its memory
            entity = nullptr; //mark it as dead to us
        }
    }
    state.worldEntities.erase(
        std::remove(state.worldEntities.begin(), state.worldEntities.end(), nullptr), //move all the nullptr entries to the end
        state.worldEntities.end() //then remove the end chunk (the nullptrs)
    );

    applyMovementInput(player, command, deltaTime); //then movement/collision after we do items (which may modify movement/collision)
    player.velocity.y += player.gravity * deltaTime;
    resolvePlayerCollisions(player, state.colliders, deltaTime);

}


//static means 'internal linkage' which means it is only given/seen by this file (its internal)
static AABB getPlayerAABB(const PlayerState& player) { //calculate the players AABB collision, relative to world coords
    AABB box;
    box.min = player.position + glm::vec3(-0.3f, 0.0f, -0.3f);
    box.max = player.position + glm::vec3(0.3f, 1.8f, 0.3f);
    return box;
}


//like handleInput in Player.cpp but uses commands and server can simulate players with it
static void applyMovementInput(PlayerState& player, const InputCommand& command, float deltaTime) {

    if (player.frozen) {
        player.velocity.x = 0.0f;
        player.velocity.z = 0.0f;
        return;
    }

    glm::vec3 flatFront = command.lookDirection; //flatfront is forward dir WITHOUT up/down tilt
    flatFront.y = 0.0f; //remove like up/down tilt
    flatFront = glm::normalize(flatFront);
    glm::vec3 flatRight = glm::normalize(glm::cross(flatFront, glm::vec3(0, 1, 0))); //get flatRight from cross product

    glm::vec3 moveDir = glm::vec3(0.0f); //a vector of our movement input (will be normalized)
    if (command.moveForward) {
        moveDir += flatFront;
    }
    if (command.moveBack) {
        moveDir -= flatFront;
    }
    if (command.moveLeft) {
        moveDir -= flatRight;
    }
    if (command.moveRight) {
        moveDir += flatRight;
    }

    if (glm::length(moveDir) > 0.0f) { //if there is movement input
        moveDir = glm::normalize(moveDir);
    }
    
    //where we want horizontal velocity to be
    glm::vec3 targetVel = moveDir * player.moveSpeed;

    //how fast we reach that target
    float accel = player.grounded ? player.groundAccel : player.airAccel; //if grounded, accell = groundedAccell, else accel = airAccel
    float t = accel * deltaTime;
    if (t > 1.0f) { //clamp it to 1.0
        t = 1.0f; //how far towards target we move this frame
    }

    player.velocity.x = glm::mix(player.velocity.x, targetVel.x, t); //like lerp
    player.velocity.z = glm::mix(player.velocity.z, targetVel.z, t);


    if (command.jump && player.grounded) { //jump
        player.velocity.y = player.jumpStrength;
    }
}


// same as resolveCollisions in Player.cpp but using commands for server
static void resolvePlayerCollisions(PlayerState& player, const std::vector<AABB>& colliders, float deltaTime) {
    player.position += player.velocity * deltaTime; //move on all axis first

    player.grounded = false; //assume airborne

    for (const AABB& c : colliders) { //for each collider
        AABB box = getPlayerAABB(player); //get our players AABB collider
        if (!aabbOverlap(box, c)) {
            continue; //no overlap so this this in the for loop
        }

        //get the depth of penetration on each axis, positive means its penetrating
        float px = glm::min(box.max.x, c.max.x) - glm::max(box.min.x, c.min.x);
        float py = glm::min(box.max.y, c.max.y) - glm::max(box.min.y, c.min.y);
        float pz = glm::min(box.max.z, c.max.z) - glm::max(box.min.z, c.min.z);


        //get the collder centres to decide the push direction
        glm::vec3 boxCentre = (box.min + box.max) * 0.5f; //get our players AABB box centre
        glm::vec3 cCentre = (c.min + c.max) * 0.5f; //get the colliders AABB box centre

        if (px < py && px < pz) { //if x is the shallowest penetration
            if (boxCentre.x < cCentre.x) { //player is on -X side
                player.position.x -= px;
            }
            else { //player is on +X side
                player.position.x += px;
            }
            player.velocity.x = 0.0f;
        }
        else if (py < pz) { //if Y is shallowest
            if (boxCentre.y < cCentre.y) { //player is below so push down
                player.position.y -= py;
            }
            else {
                player.position.y += py; //the player is grounded so much up
                player.grounded = true;
            }
            player.velocity.y = 0.0f;
        }
        else { //z is the shallowest
            if (boxCentre.z < cCentre.z) {
                player.position.z -= pz;
            }
            else {
                player.position.z += pz;
            }
            player.velocity.z = 0.0f;
        }
    }
}


#include "Pistol.h"
#include "GameState.h"
#include "AABB.h"
#include "Utils.h"
#include <iostream>
#include <cfloat>

void Pistol::use(GameState& state, int holderIndex, UseType type) {
    if (type == UseType::Primary) {
        if (primaryCooldown > 0.0f) {
            return;
        }

        if (ammo > 0) {
            ammo -= 1;
            primaryCooldown = 1.0f / primaryRate; //so if primary rate is 4, cooldown becomes 0.25.
            std::cout << "Pistol used single fire, " << ammo << " ammo left.\n";

            PlayerState& shooter = state.players[holderIndex];
            glm::vec3 origin = shooter.position + glm::vec3(0.0f, 1.7f, 0.0f); //eye height, for the hitscan ray
            glm::vec3 direction = shooter.lookDirection;

            int hitPlayer = -1;
            float hitDistance = FLT_MAX;

            for (int i = 0; i < state.players.size(); i++) {
                if (i == holderIndex) continue; //dont hit player

                AABB targetBox; //this is same as simulate()'s static getPlayerAABB()
                targetBox.min = state.players[i].position + glm::vec3(-0.3f, 0.0f, -0.3f);
                targetBox.max = state.players[i].position + glm::vec3(0.3f, 1.8f, 0.3f);

                float tHit;
                if (rayIntersectsAABB(origin, direction, targetBox, tHit)) {
                    if (tHit < hitDistance) {
                        hitDistance = tHit;
                        hitPlayer = i;
                    }
                }
            }

            bool blocked = false;
            for (const AABB& wall : state.colliders) {
                float wallHit;
                if (rayIntersectsAABB(origin, direction, wall, wallHit)) {
                    if (wallHit < hitDistance) {
                        blocked = true; // a wall is closer than the player (so in the way of the gun)
                        break;
                    }
                }
            }

            if (hitPlayer != -1 && !blocked) { //hit someone and wasnt blocked by wall
                state.players[hitPlayer].health -= damage;
                std::cout << "HIT player: " << hitPlayer << " health now: " << state.players[hitPlayer].health << "\n";
            }

        }
        else if (ammo <= 0) {
            std::cout << "Pistol used single fire, no ammo left.\n";
        }
    }
    else if (type == UseType::Secondary) {
        if (secondaryCooldown > 0.0f) {
            return;
        }

        if (ammo > 0) {
            ammo -= 1;
            secondaryCooldown = 1.0 / secondaryRate;
            std::cout << "Pistol used burst fire, " << ammo << " ammo left.\n";
        }
        else if (ammo <= 0) {
            std::cout << "Pistol used burst fire, no ammo left.\n";
        }
    }
}

void Pistol::tickInternal(float deltaTime) {
    if (primaryCooldown > 0.0f) {
        primaryCooldown -= deltaTime;
    }
    if (secondaryCooldown > 0.0f) {
        secondaryCooldown -= deltaTime;
    }
}
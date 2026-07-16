#include "Pistol.h"
#include "GameState.h"
#include <iostream>

void Pistol::use(GameState& state, int holderIndex, UseType type) {
    if (type == UseType::Primary) {
        if (primaryCooldown > 0.0f) {
            return;
        }

        if (ammo > 0) {
            ammo -= 1;
            primaryCooldown = 1.0f / primaryRate; //so if primary rate is 4, cooldown becomes 0.25.
            std::cout << "Pistol used single fire, " << ammo << " ammo left.\n";
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
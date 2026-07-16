#include "Simulation.h"
#include "Item.h"

//simulate is the server-side code


void simulate(GameState& state, int playerIndex, const InputCommand& command, float deltaTime) {
    PlayerState& player = state.players[playerIndex]; //our players PlayerState

    if (command.equipWeapon) {
        player.equipped = EquipSlot::Weapon;
    }
    if (command.equipAbility) {
        player.equipped = EquipSlot::Ability;
    }

    Item* held = (player.equipped == EquipSlot::Weapon) ? player.weapon : player.ability;

    if (held) {
        held->passiveUpdate(state, playerIndex, deltaTime);
        if (command.primaryPressed) {
            held->use(state, playerIndex, UseType::Primary);
        }
        if (command.secondaryPressed) {
            held->use(state, playerIndex, UseType::Secondary);
        }
    }
}
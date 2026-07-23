#include "RoundItems.h"

#include "ButterflyKnife.h"
#include "Pistol.h"
#include "TrafficLight.h"

void giveRoundItems(PlayerState& player) {
    delete player.weapon;
    delete player.ability;

    player.weapon = new Pistol();
    player.ability = new TrafficLight();
    if ((rand() % 2) == 1) {
        player.equipped = EquipSlot::Weapon;
    }
    else {
        player.equipped = EquipSlot::Ability;
    }
}
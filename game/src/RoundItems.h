#pragma once
#include "GameState.h"
#include <vector>
#include <string>

// we need to seperate ability and weapon items so we can randomly pick from
// each pool 
// all-lowercase, same as header name of the item
const inline std::vector<std::string> itemList = {"butterflyknife", "pistol", "trafficlight"};

void giveRoundItems(PlayerState &player, int weaponItem = -1,
                    int abilityItem = -1);
int intToItem(PlayerState &player, int itemInt, bool isWeapon);

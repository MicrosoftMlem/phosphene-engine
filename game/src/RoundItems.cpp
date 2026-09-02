#include "RoundItems.h"

#include "ButterflyKnife.h"
#include "Pistol.h"
#include "TrafficLight.h"
#include <iostream>



void giveRoundItems(PlayerState& player, int weaponItem, int abilityItem) {
  delete player.weapon;
  delete player.ability;
  player.weapon = nullptr;
  player.ability = nullptr;
  // if an invalid itemInt is supplied, we dont set player.weapon/ability even
  // though we deleted the previous one. so set to nullptr to make it safer

  std::cout << "giveRoundItems supplied with: " << weaponItem << "\n";
  
  // defaults if no item is specified
  if (weaponItem == -1) {
    player.weapon = new Pistol();
  } else {
    intToItem(player, weaponItem, true);
  }
  if (abilityItem == -1) {
    player.ability = new TrafficLight();
  } else {
    intToItem(player, abilityItem, false);
  }
  
  if (1 == 1) {
    player.equipped = EquipSlot::Weapon;
  }
  else {
    player.equipped = EquipSlot::Ability;
  }
}

int intToItem(PlayerState &player, int itemInt, bool isWeapon) {
  int itemCount = itemList.size();
  int maxIndex = itemCount - 1;

  if (itemInt > maxIndex || itemInt < 0) {
    std::cerr << "Invalid itemInt supplied: " << itemInt << " when max is: " << maxIndex << "\n";

    // GARBAGE memory will be in weaponInt and abilityInt if this happens btw
    
    if (isWeapon) {
      player.weapon = new Pistol();
      return -1;
    } else {
      player.ability = new TrafficLight();
      return -1;
    }
  } else {
    if (isWeapon) {
      player.weaponInt = itemInt;
    } else {
      player.abilityInt = itemInt;
    }
    
    std::string stringName = itemList[itemInt];
    //for ease of typing
    std::string sN = stringName;

    if (isWeapon) {
      if (sN == "butterflyknife") {
	player.weapon = new ButterflyKnife();
      } else if (sN == "pistol") {
        player.weapon = new Pistol();
      } else if (sN == "trafficlight") {
        player.weapon = new TrafficLight();
      } else {
	std::cout << "Could not find item entry for: " << sN << " when setting weapon.\n";
      }
    } else {
      // its an ability
      if (sN == "butterflyknife") {
        player.ability = new ButterflyKnife();
      } else if (sN == "pistol") {
        player.ability = new Pistol();
      } else if (sN == "trafficlight") {
        player.ability = new TrafficLight();
      } else {
	std::cout << "Could not find item entry for: " << sN << " when setting ability.\n";
        return -1;
      }
    }
  }

  return 0;
}

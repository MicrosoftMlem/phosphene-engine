#include "Bot.h"
#include "Utils.h"
#include <glm/glm.hpp>

InputCommand computeBotCommand(const GameState& state, int botIndex) {
    InputCommand command; //create a blank command

    const PlayerState& bot = state.players[botIndex];

    //get the other player id, which is the bots enemy
    int enemyIndex = (botIndex == 0) ? 1 : 0;
    const PlayerState& enemy = state.players[enemyIndex];

    //look at enemy:

    glm::vec3 botEye = bot.position + glm::vec3(0.0f, 1.7f, 0.0f);
    glm::vec3 enemyCentre = enemy.position + glm::vec3(0.0f, 0.9f, 0.0f); //aim at the enemies centre
    glm::vec3 toEnemy = glm::normalize(enemyCentre - botEye); //direction to the enemies centre
    command.lookDirection = toEnemy;

    //if we have line of sight then shoot:
    if (hasLineOfSight(botEye, enemyCentre, state.colliders)) {
        command.primaryPressed = true;
    }

    return command;
}
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cstdlib>
#include "PlayingState.h"
#include "Shader.h"
#include "Texture.h"
#include "Primitives.h"
#include "Mesh.h"
#include "InputCommand.h"
#include "Simulation.h"
#include "Pistol.h"
#include "ButterflyKnife.h"
#include "Level.h"
#include "TrafficLight.h"
#include "OBJLoader.h"
#include "Material.h"
#include "TrafficLightEntity.h"
#include "Bot.h"


PlayingState::PlayingState(GLFWwindow* window)  //this first part is the member initialisation list. it specifies HOW to construct members before the constructor is run
    : shader("basic.vert", "basic.frag"),
      texture("checker.png"),
      cubeMesh(computeNormals(Primitive::rawCubeVertices)),
      activeCamera(glm::vec3(0.0f, 0.0f, 3.0f)),
      testMesh(loadOBJ("sphere.obj")),
      trafficLightMesh(loadOBJ("trafficLightEntity.obj")),
      trafficLightTexture("trafficLightEntityTex.png"),
      playerTexture("enemy_player_checker.png")

{//then this is the constructor
    this->window = window;
    glfwSetWindowUserPointer(window, &activeCamera);


    level = loadLevel("test1v1level.level.json", &cubeMesh, &texture);



    if (!level.lights.empty()) {
        worldLightPos = level.lights[0]; //set the light pos to the first light in the level
    }
    else {
        worldLightPos = glm::vec3(5.0f, 10.0f, 5.0f); //default light pos if no lights in level
    }


    PlayerState player0; //player 0's playerstate

    if (!level.spawns.empty()) {
        player0.position = level.spawns[0]; //spawn at first spawn point
    }
    else {
        player0.position = glm::vec3(0.0f, 2.0f, 0.0f);
    }


    gameState.players.push_back(player0);


    PlayerState player1;
    player1.position = level.spawns[1];
    gameState.players.push_back(player1);

    giveRoundItems(gameState.players[0]);
    giveRoundItems(gameState.players[1]);
}


void PlayingState::update(float deltaTime) {
    uiTime += deltaTime;


        //first update game logic:
    gameState.colliders.clear();
    for (GameObject& obj : level.objects) {
        if (obj.collidable) {
            gameState.colliders.push_back(obj.getAABB());
        }
    }

    InputCommand command;
    
    command.moveForward = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
    command.moveBack = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
    command.moveLeft = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
    command.moveRight = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
    command.jump = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    command.lookDirection = activeCamera.front; //so the simulate can process where to move etc


    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        command.equipWeapon = true;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        command.equipAbility = true;
    }
    
    bool primaryIsDown = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS); //if left mouse pressed, its true. else is false
    bool secondaryIsDown = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

    command.primaryHeld = primaryIsDown;
    command.primaryPressed = primaryIsDown && !primaryWasDown; //is down and wasnt last frame

    command.secondaryHeld = secondaryIsDown;
    command.secondaryPressed = secondaryIsDown && !secondaryWasDown; //is down and wasnt down last frame

    primaryWasDown = primaryIsDown; //remember for next frame
    secondaryWasDown = secondaryIsDown;

    //reset player stats
    for (int i = 0; i < gameState.players.size(); i++) {
        resetPlayerStats(gameState.players[i]);
    }

    //world update
    updateWorld(gameState, level.spawns, deltaTime);

    //we do this after updateWorld incase updateWorld changes phase
    if (previousPhase == RoundPhase::RoundOver && gameState.phase == RoundPhase::Active) {
        giveRoundItems(gameState.players[0]);
        giveRoundItems(gameState.players[1]);
    }
    previousPhase = gameState.phase;

    //per player input/movement
    processPlayerInput(gameState, 0, command, deltaTime);

    InputCommand botCommand = computeBotCommand(gameState, 1);
    processPlayerInput(gameState, 1, botCommand, deltaTime);

    activeCamera.position = gameState.players[0].position + glm::vec3(0.0f, 1.7f, 0.0f);

}

void PlayingState::render() {
    //then render:

    shader.use();

    glClearColor(0.0f, 0.0f, 0.0f, 0.77f); //sets what colour to wipe the screen to (teal). doesnt draw yet
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //color buffer actually wipes screen with above set color. every frame starts with this to cover previous frame. and depth buffer also tells it to clear the depth buffer
    //since GL_COLOR_BUFFER_BIT and GL_DEPTH_BUFFER_BIT are bit flags, we can combine them with |


    int w, h;
    glfwGetFramebufferSize(window, &w, &h); //set w and h to the width and height of the window
    float aspect = (float)w / (float)h;

    glm::mat4 view = activeCamera.getViewMatrix(); //make the camera be at right pos and look in right dir
    glm::mat4 projection; //identity
    projection = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 100.0f); //fov = 45, aspect ratio = 800/600, near clip plane = 0.1f (close than this isnt drawn),
                                                                                        // far clip plane 100.0f (further than this isnt drawn)
    int viewLoc = glGetUniformLocation(shader.ID, "view");
    int projLoc = glGetUniformLocation(shader.ID, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


    int lightPosLoc = glGetUniformLocation(shader.ID, "lightPos");
    int lightColorLoc = glGetUniformLocation(shader.ID, "lightColor");
    int viewPosLoc = glGetUniformLocation(shader.ID, "viewPos");
    glUniform3f(lightPosLoc, worldLightPos.x, worldLightPos.y, worldLightPos.z);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // white light
    glUniform3f(viewPosLoc, activeCamera.position.x, activeCamera.position.y, activeCamera.position.z);


    for (GameObject& obj : level.objects) { //for each gameobject in the level
        obj.draw(shader);
    }

    for (WorldEntity* entity : gameState.worldEntities) {
        if (entity->getType() == EntityType::TrafficLight) {
            TrafficLightEntity* tl = static_cast<TrafficLightEntity*>(entity);

            glm::mat4 model = glm::mat4(1.0f); //identity
            model = glm::translate(model, entity->position); //move it to the entities pos
            model = glm::rotate(model, tl->getRotationY(), glm::vec3(0.0f, 1.0f, 0.0f)); //cosmetic y spin

            int modelLoc = glGetUniformLocation(shader.ID, "model"); //get the modelLoc var in the shader
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //tell the shader its pos (model)

            glm::vec3 emissiveColor;
            if (tl->getIsRed()) {
                emissiveColor = glm::vec3(1.0f, 0.0f, 0.0f);
            }
            else {
                emissiveColor = glm::vec3(0.0f, 1.0f, 0.0f);
            }


            trafficLightTexture.bind();

            glUniform3f(glGetUniformLocation(shader.ID, "tint"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(shader.ID, "emissive"), emissiveColor.r, emissiveColor.g, emissiveColor.b);
            glUniform2f(glGetUniformLocation(shader.ID, "textureScale"), 1.0f, 1.0f);

            trafficLightMesh.draw(); //draw the mesh
        }

    }


    //draw players:
    for (int i = 0; i < gameState.players.size(); i++) {
        if (i == 0) continue; //dont draw ourself

        PlayerState& p = gameState.players[i];

        glm::mat4 model = glm::translate(glm::mat4(1.0f), p.position); //make an identity and move it to the players position

        model = glm::translate(model, glm::vec3(0.0f, 0.9f, 0.0f)); //cube is centred so move it halfway up player
        model = glm::scale(model, glm::vec3(0.6f, 1.8f, 0.6f)); //scale the cube to the players size

        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        playerTexture.bind();
        glUniform3f(glGetUniformLocation(shader.ID, "tint"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shader.ID, "emissive"), 0.0f, 0.0f, 0.0f);
        glUniform2f(glGetUniformLocation(shader.ID, "textureScale"), 0.1f, 0.1f);

        cubeMesh.draw();
    }


    //                   UI PASS:
    //ui pass which is ontop so disable depth test
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND); //make it so it can do alpha
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //tell gpu how to use alpha

    //w and h are already defined earlier in the func so we reuse them:
    glfwGetFramebufferSize(window, &w, &h);

    // temporary crosshair:
    uiRenderer.drawRect(w / 2 - 1, h / 2 - 4, 2.0f, 8.0f, glm::vec4(1.0f, 1.0f, 1.0f, 0.4f), w, h); // crosshair y
    uiRenderer.drawRect(w / 2 - 4, h / 2 - 1, 8.0f, 2.0f, glm::vec4(1.0f, 1.0f, 1.0f, 0.4f), w, h); // crosshair x
    //    end of temporary crosshair

    // temporary healthBar:
    PlayerState& me = gameState.players[0];
    float healthFraction = me.health / 120.0f; //120 is max health
    if (healthFraction < 0.0f) healthFraction = 0.0f; //clamp to 0

    float barX = 40.0f;
    float barY = h - 60.0f;
    float barWidth = 300.0f;
    float barHeight = 30.0f;

    //background:
    uiRenderer.drawRect(barX, barY, barWidth, barHeight, glm::vec4(0.1f, 0.1f, 0.1f, 0.6f), w, h);

    //forground
    uiRenderer.drawRect(barX, barY, barWidth * healthFraction, barHeight, glm::vec4(1.0f, 0.2f, 0.2f, 0.8f), w, h);
    //      end of temporary healthbar


    // score pips
    float pipSize = 28.0f;
    float pipGap = 6.0f;
    float pipStartX = 40.0f;
    float pipY = 30.0f;


    //player 1 on row below
    for (int i = 0; i < gameState.roundWins[1]; i++) {
        float px = pipStartX + i * (pipSize + pipGap);

        float fireW = pipSize * 1.5f;
        float fireH = pipSize * 3.0f;
        float fireX = px - (fireW - pipSize) * 0.5f; //centre horizontally on the pip
        float fireY = pipY - (fireH - pipSize); //extend upward
        uiRenderer.drawFireRect(fireX, 30 + fireY, fireW, fireH, uiTime, i, glm::vec3(0.20000f, 0.01569f, 0.18824f), glm::vec3(0.11373f, 0.23922f, 0.02745f),  w, h);

        uiRenderer.drawCircle(px - 3, 30.0 + pipSize + pipGap - 3, pipSize + 6, glm::vec4(0.20000f, 0.01569f, 0.18824f, 1.00000f), w, h); //draw pip border
        uiRenderer.drawCircle(px, 30.0f + pipSize + pipGap, pipSize, glm::vec4(0.92941f, 0.14902f, 0.56471f, 1.00000f), w, h);
    }


    //draw player 0 after so its pips are ontop of the enemies
        // player 0 on top row
    for (int i = 0; i < gameState.roundWins[0]; i++) {
        float px = pipStartX + i * (pipSize + pipGap);


        float fireW = pipSize * 1.5f;
        float fireH = pipSize * 3.0f;
        float fireX = px - (fireW - pipSize) * 0.5f; //centre horizontally on the pip
        float fireY = pipY - (fireH - pipSize); //extend upward
        uiRenderer.drawFireRect(fireX, fireY, fireW, fireH, uiTime, i, glm::vec3(0.11373f, 0.23922f, 0.02745f), glm::vec3(0.20000f, 0.01569f, 0.18824f), w, h);

        uiRenderer.drawCircle(px - 3, pipY - 3, pipSize + 6, glm::vec4(0.11373f, 0.23922f, 0.02745f, 1.00000f), w, h); //draw pip border
        uiRenderer.drawCircle(px, pipY, pipSize, glm::vec4(0.71765f, 0.92157f, 0.20392f, 1.00000f), w, h); //draw pip after (ontop)
    }
    //     end of temporary score pips

    glEnable(GL_DEPTH_TEST); //re-enable it for the next frames 3d
    glDisable(GL_BLEND); //disable alpha for now bc idk how it will intefere with 3d
}


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
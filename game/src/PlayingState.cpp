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
#include "NetworkClient.h"
#include "RoundItems.h"


//this is the entire game state of the actual game


PlayingState::PlayingState(GLFWwindow* window, NetworkClient& networkRef)  //this first part is the member initialisation list. it specifies HOW to construct members before the constructor is run
    : shader("basic.vert", "basic.frag"),
      texture("checker.png"),
      cubeMesh(computeNormals(Primitive::rawCubeVertices)),
      activeCamera(glm::vec3(0.0f, 0.0f, 3.0f)),
      testMesh(loadOBJ("sphere.obj")),
      trafficLightMesh(loadOBJ("trafficLightEntity.obj")),
      trafficLightTexture("trafficLightEntityTex.png"),
      playerTexture("enemy_player_checker.png"),
      network(networkRef)

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
}


void PlayingState::update(float deltaTime) {
    uiTime += deltaTime;

    network.poll((float)glfwGetTime());

    int myIndex = network.getPlayerIndex();
    if (myIndex < 0 || myIndex >= (int)gameState.players.size()) return; // not recieved welcome packet yet. should never be >= players.size() (out of range)

    //colliders need to exist before sim runs (eg reconcilation)
    gameState.colliders.clear();
    for (GameObject& obj : level.objects) {
        if (obj.collidable) {
            gameState.colliders.push_back(obj.getAABB());
        }
    }

    //apply snapshot
    if (network.hasSnapshot()) {
        const Snapshot& snap = network.getSnapshot();

        if (snap.tick != lastReconciledTick) {
            lastReconciledTick = snap.tick;

            glm::vec3 predictedPosition = gameState.players[myIndex].position;

            //first do other players, from the servers state with no prediction
            for (int i = 0; i < snap.playerCount && i < (int)gameState.players.size(); i++) {
                if (i == myIndex) continue;
                gameState.players[i].position = snap.players[i].position;
                gameState.players[i].lookDirection = snap.players[i].lookDirection;
                gameState.players[i].health = snap.players[i].health;
                gameState.players[i].sliding = snap.players[i].sliding;
            }

            //now our player, rewind to the servers authorative state:
            PlayerState& me = gameState.players[myIndex];
            const PlayerSnapshot& s = snap.players[myIndex];
            me.position = s.position;
            me.velocity = s.velocity;
            me.grounded = s.grounded;
            me.health = s.health;
            me.sliding = s.sliding;
            me.dashTimeLeft = s.dashTimeLeft;
            me.dashCharges = s.dashCharges;
            me.dashRechargeTimer = s.dashRechargeTimer;
            me.dashDirection = s.dashDirection;

            //then replay everything the server hasnt seen yet:
            for (unsigned int seq = s.lastAppliedSequence + 1; seq < network.getNextSequence(); seq++) {
                InputCommand& c = commandHistory[seq % COMMAND_HISTORY_SIZE];
                if (c.sequence != seq) continue; //slot overwritten, history too short

                resetPlayerStats(me);
                processPlayerInput(gameState, myIndex, c, TICK_RATE);
            }

            gameState.roundWins[0] = snap.roundWins[0];
            gameState.roundWins[1] = snap.roundWins[1];

            //after the replay, see how wrong were we:
            glm::vec3 error = predictedPosition - gameState.players[myIndex].position;
            std::cout << "error: " << glm::length(error) << "\n";
            if (glm::length(error) > 2.0f) {
                positionError = glm::vec3(0.0f); //too big to smooth, teleport
            }
            else {
                positionError += error; //for lerping
            }
        }
    }


    //fixed timestep: build input, send and then predict
    tickAccumulator += deltaTime;
    while (tickAccumulator >= TICK_RATE) {
        tickAccumulator -= TICK_RATE;

        InputCommand command;
        
        command.moveForward = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
        command.moveBack = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
        command.moveLeft = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
        command.moveRight = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
        command.jump = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
        command.lookDirection = activeCamera.front; //so the simulate can process where to move etc
        bool dashIsDown = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
        command.dash = dashIsDown && !dashWasDown;
        dashWasDown = dashIsDown;
        command.crouch = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);


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

        unsigned int seq = network.sendCommand(command);
        command.sequence = seq;
        commandHistory[seq % COMMAND_HISTORY_SIZE] = command;

        //predict: run my input now, dont wait for server
        resetPlayerStats(gameState.players[myIndex]);

        previousPosition = gameState.players[myIndex].position;
        processPlayerInput(gameState, myIndex, command, TICK_RATE);
    }

    //per frame (runs always): 
    positionError = glm::mix(positionError, glm::vec3(0.0f), 10.0f * deltaTime);

    float targetHeight = gameState.players[myIndex].sliding ? 0.8f : 1.7f;
    cameraHeight = glm::mix(cameraHeight, targetHeight, 12.0f * deltaTime);
    //camera might not actually be at players real pos (where colliders are). bc its lerped to stop jitter (positionError is added on)
    float alpha = tickAccumulator / TICK_RATE; //0-1 how far into the next tick we are
    glm::vec3 renderPos = glm::mix(previousPosition, gameState.players[myIndex].position, alpha);
    activeCamera.position = renderPos + positionError + glm::vec3(0.0f, cameraHeight, 0.0f);

    float targetFov = (gameState.players[myIndex].sliding || (gameState.players[myIndex].dashTimeLeft > 0.0f)) ? 85.0f : 70.0f;
    currentFov = glm::mix(currentFov, targetFov, 8.0f * deltaTime);

}

void PlayingState::render() {
    int myIndex = network.getPlayerIndex();
    if (myIndex < 0) return;


    float renderTime = (float)glfwGetTime() - 0.1f; //render remote players 0.1 seconds (100 ms) in the past

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
    projection = glm::perspective(glm::radians(currentFov), aspect, 0.1f, 100.0f); //fov = 45, aspect ratio = 800/600, near clip plane = 0.1f (close than this isnt drawn),
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

    if (network.hasSnapshot()) {
        const Snapshot& snap = network.getSnapshot();

        for (int i = 0; i < snap.entityCount; i++) {
            const EntitySnapshot& e = snap.entities[i];

            if (e.type == (int)EntityType::TrafficLight) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), e.position);
                model = glm::rotate(model, e.rotationY, glm::vec3(0.0f, 1.0f, 0.0f));

                glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

                glm::vec3 emissiveColor = e.isRed ? glm::vec3(1.0f, 0.0f, 0.0f)
                                                  : glm::vec3(0.0f, 1.0f, 0.0f);
                trafficLightTexture.bind();
                glUniform3f(glGetUniformLocation(shader.ID, "tint"), 1.0f, 1.0f, 1.0f);
                glUniform3f(glGetUniformLocation(shader.ID, "emissive"), emissiveColor.r, emissiveColor.g, emissiveColor.b);
                glUniform2f(glGetUniformLocation(shader.ID, "textureScale"), 1.0f, 1.0f);

                trafficLightMesh.draw();
            }
        }
    }


    //draw players:
    for (int i = 0; i < gameState.players.size(); i++) {
        if (i == myIndex) continue; //dont draw ourself

        PlayerSnapshot interp;
        glm::vec3 drawPos;
        if (network.getInterpolatedPlayer(i, renderTime, interp)) {
            drawPos = interp.position;
        }
        else {
            drawPos = gameState.players[i].position; //falls back to newest, uninterpreted packet
        }


        glm::mat4 model = glm::translate(glm::mat4(1.0f), drawPos); //make an identity and move it to the players position

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
    PlayerState& me = gameState.players[myIndex];
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
    float pipGap = 8.0f;
    float pipStartX = 40.0f;
    float pipY = 30.0f;

    int otherIndex = (myIndex == 0) ? 1 : 0;

    //player 1 on row below
    for (int i = 0; i < gameState.roundWins[otherIndex]; i++) {
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
    for (int i = 0; i < gameState.roundWins[myIndex]; i++) {
        float px = pipStartX + 12 + i * (pipSize + pipGap);


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


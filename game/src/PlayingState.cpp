#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "PlayingState.h"
#include "Shader.h"
#include "Texture.h"
#include "Primitives.h"
#include "Mesh.h"
#include "InputCommand.h"
#include "Simulation.h"
#include "Pistol.h"


PlayingState::PlayingState(GLFWwindow* window)  //this first part is the member initialisation list. it specifies HOW to construct members before the constructor is run
    : shader("basic.vert", "basic.frag"),
      texture("checker.png"),
      cubeMesh(computeNormals(Primitive::rawCubeVertices)),
      activeCamera(glm::vec3(0.0f, 0.0f, 3.0f)),
      player(glm::vec3(0.0f, 0.0f, 3.0f))

{//then this is the constructor
    this->window = window;
    glfwSetWindowUserPointer(window, &activeCamera);
    worldLightPos = glm::vec3(5.0f, 10.0f, 5.0f);

    //a wide, thin floor in the centre of world, 0.5 below (y)
    level.push_back(GameObject(&cubeMesh, &texture, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(100.0f, 1.0f, 100.0f), glm::vec2(12.0f, 12.0f)));

    //move objects to test with:
    level.push_back(GameObject(&cubeMesh, &texture, glm::vec3(3.0f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)));
    level.push_back(GameObject(&cubeMesh, &texture, glm::vec3(-2.0f, 0.5f, 4.0f), glm::vec3(1.0f, 2.0f, 1.0f), glm::vec2(1.0f, 1.0f)));
    level.push_back(GameObject(&cubeMesh, &texture, glm::vec3(0.0f, 1.0f, -5.0f), glm::vec3(4.0f, 2.0f, 1.0f), glm::vec2(4.0f, 1.0f)));

    PlayerState player0; //player 0's playerstate
    player0.position = glm::vec3(0.0f);
    player0.weapon = new Pistol();
    gameState.players.push_back(player0);
}


void PlayingState::update(float deltaTime) {
        //first update game logic:
    std::vector<AABB> colliders;
    for (GameObject& obj : level) {
        colliders.push_back(obj.getAABB());
    }
    player.update(window, activeCamera, colliders, deltaTime);

    InputCommand command;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        command.equipWeapon = true;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        command.equipAbility = true;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        command.primaryPressed = true;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        command.secondaryPressed = true;
    }


    simulate(gameState, 0, command, deltaTime);
}

void PlayingState::render() {
    //then render:

    shader.use();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //sets what colour to wipe the screen to (teal). doesnt draw yet
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


    for (GameObject& obj : level) { //for each gameobject in the level
        obj.draw(shader);
    }
}
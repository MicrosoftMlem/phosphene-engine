#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "GameStateBase.h"
#include "GameState.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Level.h"
#include "UIRenderer.h"
#include "NetworkClient.h"


class PlayingState : public GameStateBase {
public:
    PlayingState(GLFWwindow* window, NetworkClient& network); //constructor
    void update(float deltaTime) override;
    void render() override;

private:
    GLFWwindow* window;
    NetworkClient& network;

    GameState gameState;
    Camera activeCamera;
    glm::vec3 worldLightPos;
    Shader shader;
    Texture texture;
    Texture playerTexture;
    Mesh cubeMesh;

    Mesh testMesh;

    Mesh trafficLightMesh;
    Texture trafficLightTexture;
    
    Level level;

    UIRenderer uiRenderer;

    bool primaryWasDown = false; //for edge detecting mouse click
    bool secondaryWasDown = false;
    bool dashWasDown = false;

    float cameraHeight = 1.7f;
    float currentFov = 70.0f;


    float uiTime = 0.0f;

    float tickAccumulator = 0.0f;
    static constexpr float TICK_RATE = 1.0f / 60.0f;

    static constexpr int COMMAND_HISTORY_SIZE = 64;
    InputCommand commandHistory[COMMAND_HISTORY_SIZE];
};
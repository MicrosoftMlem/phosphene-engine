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


class PlayingState : public GameStateBase {
public:
    PlayingState(GLFWwindow* window); //constructor
    void update(float deltaTime) override;
    void render() override;

private:
    GLFWwindow* window;
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

    RoundPhase previousPhase = RoundPhase::Active;

    float uiTime = 0.0f;
};

void giveRoundItems(PlayerState& player);
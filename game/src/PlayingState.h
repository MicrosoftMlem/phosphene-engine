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
    Mesh cubeMesh;
    
    Level level;

    bool primaryWasDown = false; //for edge detecting mouse click
    bool secondaryWasDown = false;
};
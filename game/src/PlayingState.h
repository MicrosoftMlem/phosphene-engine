#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "GameStateBase.h"
#include "GameState.h"
#include "Camera.h"
#include "Player.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "GameObject.h"


class PlayingState : public GameStateBase {
public:
    PlayingState(GLFWwindow* window); //constructor
    void update(float deltaTime) override;
    void render() override;

private:
    GLFWwindow* window;
    GameState gameState;
    Camera activeCamera;
    Player player;
    glm::vec3 worldLightPos;
    Shader shader;
    Texture texture;
    Mesh cubeMesh;
    std::vector<GameObject> level;
};
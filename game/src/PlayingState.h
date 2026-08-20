#pragma once
#include "Camera.h"
#include "GameObject.h"
#include "GameState.h"
#include "GameStateBase.h"
#include "Level.h"
#include "Mesh.h"
#include "NetworkClient.h"
#include "Shader.h"
#include "Texture.h"
#include "UIRenderer.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class PlayingState : public GameStateBase {
public:
  PlayingState(GLFWwindow *window, NetworkClient &network); // constructor
  void update(float deltaTime) override;
  void render() override;

private:
  GLFWwindow *window;
  NetworkClient &network;

  GameState gameState;
  Camera activeCamera;
  glm::vec3 worldLightPos;
  Shader shader;
  Texture texture;
  Texture playerTexture;
  Mesh cubeMesh;

  Mesh testMesh;
  // TODO: all meshes need to be loaded in here. this should be moved into the
  // entity itself
  Mesh trafficLightMesh;
  Texture trafficLightAlbedo;
  Texture trafficLightEmissive;

  Level level;

  UIRenderer uiRenderer;

  bool primaryWasDown = false; // for edge detecting mouse click
  bool secondaryWasDown = false;
  bool dashWasDown = false;

  float cameraHeight = 1.7f;
  float currentFov = 70.0f;

  float uiTime = 0.0f;

  float tickAccumulator = 0.0f;
  static constexpr float TICK_RATE = 1.0f / 60.0f;

  static constexpr int COMMAND_HISTORY_SIZE = 64;
  InputCommand commandHistory[COMMAND_HISTORY_SIZE];

  unsigned int lastReconciledTick = 0;
  glm::vec3 positionError = glm::vec3(0.0f);
  glm::vec3 previousPosition = glm::vec3(0.0f);
};

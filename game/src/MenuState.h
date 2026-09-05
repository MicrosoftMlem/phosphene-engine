#pragma once
#include "GameStateBase.h"
#include "UIRenderer.h"
#include <string>

struct GLFWwindow; //forward declare

class MenuState : public GameStateBase {
public:
  MenuState(GLFWwindow* window);
  void update(float deltaTime) override;
  void render() override;

private:
  std::string testTextureStringName = "testMenuTex";

  Texture testTex = nullptr;
  
  UIRenderer uiRenderer;
  GLFWwindow* window;
};

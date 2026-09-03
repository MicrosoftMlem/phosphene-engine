#include "shaderStore.h"

#include "Shader.h"
#include <unordered_map>
#include <string>
#include <iostream>

//static means private to this file
static std::unordered_map<std::string, Shader> shaderMap;

// shaderMap["key"] == Shader("path");
// would first make Shader with its default constructor,
// and then construct it with "path". but Shader doesnt have
// a default constructor so it breaks. .emplace fixes that

// gold_blue_cellular.frag is not included

void initShaders() {
  shaderMap.emplace("basicShader", Shader("basic.vert", "basic.frag"));
  shaderMap.emplace("uiFireShader", Shader("basic.vert", "ui_fire.frag"));
  shaderMap.emplace("uiCircleShader", Shader("basic.vert", "ui_circle.frag"));
  shaderMap.emplace("uiShader", Shader("ui.vert", "ui.frag"));
}

Shader *getShader(const std::string &shaderName) {
  auto it = shaderMap.find(shaderName);
  if (it == shaderMap.end()) {
    std::cerr << "Couldn't find shader: " << shaderName << " in map.\n";
    // this should be a debug shader, like the normal-face one
    auto fb = shaderMap.find("basicShader");
    if (fb == shaderMap.end()) {
      std::cerr << "FATAL: basicShader not in map. Aborting.\n";
      std::abort();
    }
    return &fb->second;
  }

  return &it->second;
}

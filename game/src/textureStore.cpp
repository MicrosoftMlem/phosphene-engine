#include "textureStore.h"

#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

//static means private to this file
static std::unordered_map<std::string, Texture> textureMap;

static std::vector<std::string> deprecatedTextures;

// textureMap["key"] == Texture("path");
// would first make Texture with its default constructor,
// and then construct it with "path". but Texture doesnt have
// a default constructor so it breaks. .emplace fixes that

void initTextures() {
  textureMap.emplace("checkerTex", Texture("checker.png"));
  textureMap.emplace("enemyCheckerTex", Texture("enemy_player_checker.png"));
  textureMap.emplace("noiseTex", Texture("noise.png"));
  textureMap.emplace("trafficLightEntityAlbedoTex", Texture("TrafficLightEntityAlbedo.png"));
  textureMap.emplace("trafficLightEntityEmissiveTex",
                     Texture("TrafficLightEntityEmissive.png"));
  textureMap.emplace("trafficLightEntityTexTex",
                     Texture("trafficLightEntityTex.png"));
  textureMap.emplace("vignetteTex", Texture("vignette.png"));

  textureMap.emplace("missingTextureTex", Texture("missingTexture.png"));
  markTextureDeprecated("trafficLightEntityTexTex");
}

void markTextureDeprecated(const std::string &stringName) {
  auto it = textureMap.find(stringName);

  if (it == textureMap.end()) {
    std::cerr << "Invalid texture marked as deprecated: " << stringName << "\n";
  } else {
    deprecatedTextures.push_back(stringName);
  }
}

Texture *getTexture(const std::string &textureName) {
  auto it = textureMap.find(textureName);
  if (it == textureMap.end()) {
    std::cerr << "Couldn't find texture: " << textureName << " in map.\n";
    auto fb = textureMap.find("missingTextureTex");
    if (fb == textureMap.end()) {
      std::cerr << "FATAL: missingTextureTex not in map. Aborting.\n";
      std::abort();
    }
    return &fb->second;
  }

  auto depIt = std::find(deprecatedTextures.begin(), deprecatedTextures.end(),
                         textureName);
  if (depIt != deprecatedTextures.end()) {
    // its deprecated, show a warning
    std::cerr << "Deprecated texture got: " << textureName << "\n";
  }

  return &it->second;
}

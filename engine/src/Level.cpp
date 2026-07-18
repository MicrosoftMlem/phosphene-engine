#include "Level.h"
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
using json = nlohmann::json; //alais the namespace (so saying 'json' is same as 'nlohmann::json')

static glm::vec3 readVec3(const json& array) { //turns an array with 3 entries into a vector3
    return glm::vec3(array[0], array[1], array[2]);
}

Level loadLevel(const std::string& path, Mesh* mesh, Texture* texture) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "Failed to open level: " << path << "\n";
        return Level();
    }
    json data = json::parse(file);

    Level level;

    for (const auto& box : data["boxes"]) { //auto bc its a long nlohmann json type i dont wanna write
        glm::vec3 min = readVec3(box["min"]); //get min
        glm::vec3 max = readVec3(box["max"]); //get max

        glm::vec3 centre = (min + max) * 0.5f; //gets the centre of the box
        glm::vec3 size = max - min;
        glm::vec2 textureScale = glm::vec2(size.x, size.z) / 8.0f; //scale the texture to the size of the box

        level.objects.push_back(GameObject(mesh, texture, centre, size, textureScale));
    }

    for (const auto& spawn : data["spawns"]) {
        level.spawns.push_back(readVec3(spawn["pos"])); //just push back the pos
    }

    for (const auto& items : data["items"]) {
        level.itemSpawns.push_back(readVec3(items["pos"])); //just push back the pos of items
    }

    for (const auto& light : data["lights"]) {
        level.lights.push_back(readVec3(light["pos"])); //just push back the pos of lights
    }

    return level;
}
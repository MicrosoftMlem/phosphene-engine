#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "GameObject.h"
#include "Mesh.h"
#include "Texture.h"

struct Level {
    std::vector<GameObject> objects;
    std::vector<glm::vec3> spawns;
    std::vector<glm::vec3> itemSpawns;
    std::vector<glm::vec3> lights;
};

Level loadLevel(const std::string& path, Mesh* mesh, Texture* texture);
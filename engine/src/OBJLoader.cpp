#include "OBJLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

std::vector<float> loadOBJ(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "Failed to open OBJ: " << path << "\n";
        return {};
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<float> vertices; //the flat output


    std::string line;
    while (std::getline(file, line)) {
        std::istringstream stream(line);
        std::string prefix;
        stream >> prefix; //the first token (v, vt, vn, f, etc)

        if (prefix == "v") {
            //read 3 floats into vec3 and push to positions
            float x, y, z;
            stream >> x >> y >> z; //read the next 3 tokens into the floats
            positions.push_back(glm::vec3(x, y, z));
        }
        else if (prefix == "vt") {
            //read 2 floats into vec2 and push to uvs
            float x, y;
            stream >> x >> y;
            uvs.push_back(glm::vec2(x, y));
        }
        else if (prefix == "vn") {
            //read 3 floats into a vec3, push to normals
            float x, y, z;
            stream >> x >> y >> z;
            normals.push_back(glm::vec3(x, y, z));
        }
        else if (prefix == "f") {
            for (int i = 0; i < 3; i++) { //3 corners
                std::string corner;
                stream >> corner;


                std::istringstream cornerStream(corner);
                std::string vStr, vtStr, vnStr;
                std::getline(cornerStream, vStr, '/');
                std::getline(cornerStream, vtStr, '/');
                std::getline(cornerStream, vnStr, '/');

                int vi = std::stoi(vStr) - 1; //they were '1' based so we make them 0 based
                int ti = std::stoi(vtStr) - 1;
                int ni = std::stoi(vnStr) - 1;

                glm::vec3 pos = positions[vi];
                glm::vec2 uv = uvs[ti];
                glm::vec3 nrm = normals[ni];

                vertices.push_back(pos.x); //and finally put it in the format our engine uses (our meshes VAO layout)
                vertices.push_back(pos.y);
                vertices.push_back(pos.z);
                vertices.push_back(uv.x);
                vertices.push_back(uv.y);
                vertices.push_back(nrm.x);
                vertices.push_back(nrm.y);
                vertices.push_back(nrm.z);

            }
        }
    }

    // std::cout << "positions: " << positions.size()
    //           << " uvs: " << uvs.size()
    //           << " normals: " << normals.size() << "\n";
    
    std::cout << "vertices floats: " << vertices.size() << "\n";
    
    return vertices;
}
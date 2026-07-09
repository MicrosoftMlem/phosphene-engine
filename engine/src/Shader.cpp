#include "Shader.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

void checkErrors(unsigned int object, std::string type) {
    int success; //int for storing if there was error or not. 1 = no error, 0 = error
    char infoLog[512];
    if (type == "VERTEX" || type == "FRAGMENT") {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success); //asks OpenGL the outcome of GL_COMPILE_STATUS (did it compile) of 'object' and stores result in success
        if (success == 0) { //if it wasnt a successful compile
            glGetShaderInfoLog(object, 512, nullptr, infoLog); //get the OpenGL compile error logs and store it in infoLog
            std::cout << "SHADER ERROR >> " << infoLog << "  ON OBJECT  " << object << "\n";
        }
    }
    else if (type == "PROGRAM") {
        glGetProgramiv(object, GL_LINK_STATUS, &success); //same as above but note 'glGetProgramiv' instead of 'glGetShaderiv'. and its GL_LINK_STATUS for program linking
        if (success == 0) {
            glGetProgramInfoLog(object, 512, nullptr, infoLog); //note glGetProgramInfoLog not glGetShaderInfoLog
            std::cout << "PROGRAM ERROR >> " << infoLog << "  ON OBJECT  " << object << "\n";
        }
    }
    else {
        std::cout << "checkErrors CALLED WITH INVALID TYPE\n";
    }
}

Shader::Shader(const char* vertPath, const char* fragPath) {
    std::ifstream vFile(vertPath); //ifstring opens a file. on this line we open vertPath and store the ifstream in the var vFile
    std::ifstream fFile(fragPath);
    if (!vFile.is_open()) { //check that it successfully loaded the files.
        std::cout << "(VERTEX) SHADER FILE NOT FOUND: " << vertPath << "\n";
    }
    if (!fFile.is_open()) {
        std::cout << "(FRAGMENT) SHADER FILE NOT FOUND: " << fragPath << "\n";
    }


    std::stringstream vStream, fStream; //create the v and f stringstreams
    vStream << vFile.rdbuf(); //rdbuf dumps files into stringstreams. so here we dump the contents of vFile into the vStream stringstream.
    fStream << fFile.rdbuf();
    std::string vCode = vStream.str(); //convert the stringstream to a string
    std::string fCode = fStream.str();
    const char* vSource = vCode.c_str(); //converts the string to a 'const char*' which is the format GL wants it in
    const char* fSource = fCode.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vSource, nullptr);
    glCompileShader(vertex);
    checkErrors(vertex, "VERTEX");

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fSource, nullptr);
    glCompileShader(fragment);
    checkErrors(fragment, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(ID);
}
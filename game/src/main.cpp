#include <glad/glad.h> //glad has to be included before glfw3 bc glfw3 will try use glad so glad has to be defined
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> //the core types like mat4, vec3
#include <glm/gtc/matrix_transform.hpp> //the builders translate, rotate, scale
#include <glm/gtc/type_ptr.hpp> // a bridge to give glms matrix to OpenGL
#include <iostream>
#include <vector>
#include <enet/enet.h>
//#include "Shader.h"
//#include "Texture.h"
//#include "Camera.h"
//#include "Mesh.h"
//#include "Player.h"
//#include "GameObject.h"
//#include "GameState.h"
//#include "Pistol.h"
//#include "Item.h"
//#include "InputCommand.h"
#include "Simulation.h"
#include "PlayingState.h"
#include "GameStateBase.h"
#include "OBJLoader.h"
#include "Server.h"
#include "NetworkClient.h"
#include "StateManager.h"

/*
when we are telling the gpu to do stuff we have 2 things:
- VBO (vertex buffer object). this is the actual data of where vertexes of shapes are
- VAO (vertex array object). this tells the gpu how to interpret/handle the VBO. and then the gpu remembers it so u just need to do it in init
            its like saying 'read the VBO like this' and then we 'bind' it to draw so when we draw the gpu checks the VAO automatically.

when we want coordinates, atleast for now we use NDC - Normalised Device Coordinates.
- as in the name, its normalised, so -1.0 to 1.0
- later on we can map this to real coords (GL still uses NDC, we can just use real coords at a higher level. like a wrapper.)
*/



//GL will call this every time the window is resized and will pass the args
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height); //tells GL to render into this whole area
}



float lastX = 400.0f; //the mouse position last frame
float lastY = 300.0f;
bool firstMouse = true; //a flag to handle the very first mouse event cleanly

float deltaTime = 0.0f;
float lastFrame = 0.0f;


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    Camera* camera = (Camera*)glfwGetWindowUserPointer(window); //get the stored camera pointer (its stored as a void* so we cast it to a camera*)
    if (camera == nullptr) return;

    if (firstMouse) { //stop the mouse from jumping at startup bc the cursor could be anywhere
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX; //how far mouse has moved horizontally since last frame
    float yoffset = lastY - ypos; //reversed: y goes from bottom to top as screen y increases DOWNWARD //how far the mouse has moved vertically since last frame
    lastX = xpos;
    lastY = ypos;

    camera->processMouse(xoffset, yoffset); //its '->' not '::' bc we are doing the function of the specific camera pointer so 'deference the pointer and call the method'
}



int main(int argc, char** argv) {
    bool isServer = (argc > 1 && std::string(argv[1]) == "--server");

    if (enet_initialize() != 0) { std::cout << "ENet init failed\n"; return 1; }

    if (isServer) {
        runServer();
        enet_deinitialize();
        return 0; //so we stop here and dont run all the rendering stuff bc server doesnt need that
    }


    NetworkClient network;
    if (!network.connectToServer("127.0.0.1", 7777)) {
        std::cout << "Could not connect, exiting\n";
        return 1;
    }



    glfwInit(); //starts the library

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //these 3 set up what version we want. core is modern

    GLFWwindow* window = glfwCreateWindow(800, 600, "Phosphene Engine", nullptr, nullptr); //width, height, title, fullscreen-monitor, window_sharing (idk)
    if (window == nullptr) { //above statement returns null if failed, pointer if succeeded.
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // basically says 'opengl commands from now on will go to this window'

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { //glad goes and fetches the function pointers. it asks glfw where they are // INIT GLAD
                                                                // so glfw context has to be set by now (line 17). also must be before any gl call.
        std::cout << "Failed to init glad\n";
        return -1;
    }



    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //tell GLFW to call that func when the window resizes
    glfwSetCursorPosCallback(window, mouse_callback);


    glEnable(GL_DEPTH_TEST); //enable depth testing
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //hides cursor and locks it so it wont come out window when doing camera moving w/ mouse


    StateManager states(window);
    states.requestMenu();


    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime(); //seconds since start
        deltaTime = currentFrame - lastFrame;
        if (deltaTime > 0.05) deltaTime = 0.05; //clamp it to prevent bad behaviour on lag spikes etc
        lastFrame = currentFrame;

        
	states.update(deltaTime);
        states.render();


        glfwSwapBuffers(window); //swaps a back buffer ontop which should have game frame drawn to. back buffers reduce tearing etc
        glfwPollEvents(); //check for mouse, keyboard etc. otherwise OS will think its hung
    }

    glfwTerminate(); //tidy shutdown
    return 0;
}

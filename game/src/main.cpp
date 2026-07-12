#include <glad/glad.h> //glad has to be included before glfw3 bc glfw3 will try use glad so glad has to be defined
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> //the core types like mat4, vec3
#include <glm/gtc/matrix_transform.hpp> //the builders translate, rotate, scale
#include <glm/gtc/type_ptr.hpp> // a bridge to give glms matrix to OpenGL
#include <iostream>
#include <vector>
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Mesh.h"
#include "Player.h"
#include "GameObject.h"

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



std::vector<float> computeNormals(const std::vector<float>& verts) {
    std::vector<float> out;
    int stride = 5; //5 floats per vertex (x,y,z,u,v)


    for (int i = 0; i < verts.size(); i += stride * 3) { //go thru 3 verts at a time (so basically: for each triangle)
        //get the 3 positions:
        glm::vec3 A(verts[i + 0], verts[i + 1], verts[i + 2]);
        glm::vec3 B(verts[i + stride + 0], verts[i + stride + 1], verts[i + stride + 2]);
        glm::vec3 C(verts[i + stride*2 + 0], verts[i + stride*2 + 1], verts[i + stride*2 + 2]);

        //compute the faces normal
        glm::vec3 edge1 = B - A;
        glm::vec3 edge2 = C - A;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        
        //output the 3 vertices with the normals appended
        for (int v = 0; v < 3; v++) {
            int base = i + v * stride;
            
            out.push_back(verts[base + 0]); //X
            out.push_back(verts[base + 1]); //Y
            out.push_back(verts[base + 2]); //Z
            out.push_back(verts[base + 3]); //U
            out.push_back(verts[base + 4]); //V
            
            //append the normal:
            out.push_back(normal.x);
            out.push_back(normal.y);
            out.push_back(normal.z);
        }
    }
    return out;
}








int main() {
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



    //remember to write an MD on this
    //write about A, B, C and adding D
    std::vector<float> rawVertices = { //cube vertices in local space (the cubes shape defined around its own origin)

        // BACK
        -0.5, -0.5, -0.5, 0.0, 0.0, //back triangle A
        0.5, -0.5, -0.5, 1.0, 0.0,
        0.5, 0.5, -0.5, 1.0, 1.0,

        -0.5, -0.5, -0.5, 0.0, 0.0, //back triangle B
        0.5, 0.5, -0.5, 1.0, 1.0,
        -0.5, 0.5, -0.5, 0.0, 1.0,

        // FRONT
        -0.5, -0.5, 0.5, 0.0, 0.0, //front trangle A
        0.5, -0.5, 0.5, 1.0, 0.0,
        0.5, 0.5, 0.5, 1.0, 1.0,

        -0.5, -0.5, 0.5, 0.0, 0.0, //front trangle B
        0.5, 0.5, 0.5, 1.0, 1.0,
        -0.5, 0.5, 0.5, 0.0, 1.0,

        // LEFT
        -0.5, -0.5, -0.5, 0.0, 0.0, //A //left triangle A
        -0.5, -0.5, 0.5, 1.0, 0.0, //B
        -0.5, 0.5, 0.5, 1.0, 1.0,  //C

        -0.5, -0.5, -0.5, 0.0, 0.0, //A //left triangle B
        -0.5, 0.5, 0.5, 1.0, 1.0, //C
        -0.5, 0.5, -0.5, 0.0, 1.0, //D
        
        // RIGHT
        0.5, -0.5, -0.5, 0.0, 0.0, //A //right triangle A
        0.5, -0.5, 0.5, 1.0, 0.0,  //B
        0.5, 0.5, 0.5, 1.0, 1.0,   //C

        0.5, -0.5, -0.5, 0.0, 0.0, //A //right triangle B
        0.5, 0.5, 0.5, 1.0, 1.0,   //C
        0.5, 0.5, -0.5, 0.0, 1.0,  //D

        // BOTTOM
        -0.5, -0.5, -0.5, 0.0, 0.0, //A //bottom triangle A
        -0.5, -0.5, 0.5, 1.0, 0.0,  //B
        0.5, -0.5, 0.5, 1.0, 1.0,   //C

        -0.5, -0.5, -0.5, 0.0, 0.0, //A //bottom triangle B
        0.5, -0.5, 0.5, 1.0, 1.0,   //C
        0.5, -0.5, -0.5, 0.0, 1.0,  //D

        //TOP
        -0.5, 0.5, -0.5, 0.0, 0.0, //top triangle A
        -0.5, 0.5, 0.5, 1.0, 0.0,
        0.5, 0.5, 0.5, 1.0, 1.0,

        -0.5, 0.5, -0.5, 0.0, 0.0, //top triangle B
        0.5, 0.5, 0.5, 1.0, 1.0,
        0.5, 0.5, -0.5, 0.0, 1.0,
    };

    std::vector<float> vertices = computeNormals(rawVertices);


    Shader shader("basic.vert", "basic.frag");
    Texture texture("checker.png");
    Mesh cubeMesh(vertices);


    std::vector<GameObject> level;

    //a wide, thin floor in the centre of world, 0.5 below (y)
    level.push_back(GameObject(&cubeMesh, &texture, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(100.0f, 1.0f, 100.0f), glm::vec2(12.0f, 12.0f)));

    //move objects to test with:
    level.push_back(GameObject(&cubeMesh, &texture, glm::vec3(3.0f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)));
    level.push_back(GameObject(&cubeMesh, &texture, glm::vec3(-2.0f, 0.5f, 4.0f), glm::vec3(1.0f, 2.0f, 1.0f), glm::vec2(1.0f, 1.0f)));
    level.push_back(GameObject(&cubeMesh, &texture, glm::vec3(0.0f, 1.0f, -5.0f), glm::vec3(4.0f, 2.0f, 1.0f), glm::vec2(4.0f, 1.0f)));

    glm::vec3 worldLightPos = glm::vec3(5.0f, 10.0f, 5.0f);


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //tell GLFW to call that func when the window resizes
    glfwSetCursorPosCallback(window, mouse_callback);


    glEnable(GL_DEPTH_TEST); //enable depth testing
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //hides cursor and locks it so it wont come out window when doing camera moving w/ mouse


    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); //create camera
    glfwSetWindowUserPointer(window, &camera); //store the camera pointer in glfw's single free pointer storage (for anything)


    Player player(glm::vec3(0.0f, 40.0f, 3.0f)); //create/spawn player


    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime(); //seconds since start
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //first update game logic:
        std::vector<AABB> colliders;
        for (GameObject& obj : level) {
            colliders.push_back(obj.getAABB());
        }
        player.update(window, camera, colliders, deltaTime);

        //then render:

        shader.use();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //sets what colour to wipe the screen to (teal). doesnt draw yet
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //color buffer actually wipes screen with above set color. every frame starts with this to cover previous frame. and depth buffer also tells it to clear the depth buffer
        //since GL_COLOR_BUFFER_BIT and GL_DEPTH_BUFFER_BIT are bit flags, we can combine them with |


        int w, h;
        glfwGetFramebufferSize(window, &w, &h); //set w and h to the width and height of the window
        float aspect = (float)w / (float)h;

        glm::mat4 view = camera.getViewMatrix(); //make the camera be at right pos and look in right dir
        glm::mat4 projection; //identity
        projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f); //fov = 45, aspect ratio = 800/600, near clip plane = 0.1f (close than this isnt drawn),
                                                                                            // far clip plane 100.0f (further than this isnt drawn)
        int viewLoc = glGetUniformLocation(shader.ID, "view");
        int projLoc = glGetUniformLocation(shader.ID, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


        int lightPosLoc = glGetUniformLocation(shader.ID, "lightPos");
        int lightColorLoc = glGetUniformLocation(shader.ID, "lightColor");
        int viewPosLoc = glGetUniformLocation(shader.ID, "viewpos");
        glUniform3f(lightPosLoc, worldLightPos.x, worldLightPos.y, worldLightPos.z);
        glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // white light
        glUniform3f(viewPosLoc, camera.position.x, camera.position.y, camera.position.z);


        for (GameObject& obj : level) { //for each gameobject in the level
            obj.draw(shader);
        }


        glfwSwapBuffers(window); //swaps a back buffer ontop which should have game frame drawn to. back buffers reduce tearing etc
        glfwPollEvents(); //check for mouse, keyboard etc. otherwise OS will think its hung
    }

    glfwTerminate(); //tidy shutdown
    return 0;
}
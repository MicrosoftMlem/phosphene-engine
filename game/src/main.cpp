#include <glad/glad.h> //glad has to be included before glfw3 bc glfw3 will try use glad so glad has to be defined
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> //the core types like mat4, vec3
#include <glm/gtc/matrix_transform.hpp> //the builders translate, rotate, scale
#include <glm/gtc/type_ptr.hpp> // a bridge to give glms matrix to OpenGL
#include <iostream>
#include "Shader.h"

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


void processInput(GLFWwindow* window, glm::vec3& cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp) { //we use & so we get a reference which means when we
                                                                                                            //modify cameraPos, we change the value in main,
                                                                                                            //not a copy. we dont use & for Front and Up
                                                                                                            //bc they are only ever read, not changed
    float speed = 0.05f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){ //polls if W is held down
        cameraPos += speed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        cameraPos -= speed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed; //the direction perpedicular to front and up = right (and left if negative)
                                                                                // we normalise it bc otherwise strafe speed would change based on angles 
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    }
}

float yaw = -90.0f; //0 points down +x so -90 points down -z
float pitch = 0.0f;
float lastX = 400.0f; //the mouse position last frame
float lastY = 300.0f;
bool firstMouse = true; //a flag to handle the very first mouse event cleanly
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); //make camera look forward


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { //stop the mouse from jumping at startup bc the cursor could be anywhere
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX; //how far mouse has moved horizontally since last frame
    float yoffset = lastY - ypos; //reversed: y goes from bottom to top as screen y increases DOWNWARD //how far the mouse has moved vertically since last frame
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity; //raw mouse movement is to fast so scale it down
    yoffset *= sensitivity;

    yaw += xoffset; //accumulate the angles
    pitch += yoffset;

    if (pitch > 89.0f) { //clamp the pitch to stop you from looking past straight up
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    glm::vec3 direction; //this math somehow converts angles into direction vectors
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

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
    float vertices[] = { //cube vertices in local space (the cubes shape defined around its own origin)

        // BACK
        -0.5, -0.5, -0.5, //back triangle A
        0.5, -0.5, -0.5,
        0.5, 0.5, -0.5,

        -0.5, -0.5, -0.5, //back triangle B
        0.5, 0.5, -0.5,
        -0.5, 0.5, -0.5,

        // FRONT
        -0.5, -0.5, 0.5, //front trangle A
        0.5, -0.5, 0.5,
        0.5, 0.5, 0.5,

        -0.5, -0.5, 0.5, //front trangle B
        0.5, 0.5, 0.5,
        -0.5, 0.5, 0.5,

        // LEFT
        -0.5, -0.5, -0.5, //A //left triangle A
        -0.5, -0.5, 0.5,  //B
        -0.5, 0.5, 0.5,   //C

        -0.5, -0.5, -0.5, //A //left triangle B
        -0.5, 0.5, 0.5,   //C
        -0.5, 0.5, -0.5,  //D
        
        // RIGHT
        0.5, -0.5, -0.5, //A //right triangle A
        0.5, -0.5, 0.5,  //B
        0.5, 0.5, 0.5,   //C

        0.5, -0.5, -0.5, //A //right triangle B
        0.5, 0.5, 0.5,   //C
        0.5, 0.5, -0.5,  //D

        // BOTTOM
        -0.5, -0.5, -0.5, //A //bottom triangle A
        -0.5, -0.5, 0.5,  //B
        0.5, -0.5, 0.5,   //C

        -0.5, -0.5, -0.5, //A //bottom triangle B
        0.5, -0.5, 0.5,   //C
        0.5, -0.5, -0.5,  //D

        //TOP
        -0.5, 0.5, -0.5, //top triangle A
        -0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,

        -0.5, 0.5, -0.5, //top triangle B
        0.5, 0.5, 0.5,
        0.5, 0.5, -0.5,
    };

    unsigned int VAO, VBO; //create the VAO and VBO
    glGenVertexArrays(1, &VAO); //and get their IDs
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO); //set this VAO as the active one (bind it) - read below
    /*
    OpenGL is a state machine, you bind an object (such as the VAO) to make it active, then subsequent calls affect whatever is bound
    so unlike normal c++ instead of passing objects to functions, we bind them and the calls act on the bound one
    */

    glBindBuffer(GL_ARRAY_BUFFER, VBO); //bind the VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //upload our triangles vertices into GPU memory
    // GL_STATIC_DRAW tells the gpu that this data wont change often which allows the GPU to optimise

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //as said at the top of the file, this is the VAO recipe itself.
    //slot 0 (matches the shader), 3 floats per vertex, they're floats, dont normalise them, each vertex is 3*sizeof(float) apart (the stride) (12 bytes)
    // , starting at offset 0
    glEnableVertexAttribArray(0); //turn slot 0 on bc its off by default.


    Shader shader("basic.vert", "basic.frag");


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //tell GLFW to call that func when the window resizes
    glfwSetCursorPosCallback(window, mouse_callback);

    glEnable(GL_DEPTH_TEST); //enable depth testing
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //hides cursor and locks it so it wont come out window when doing camera moving w/ mouse


    //we setup the camera outside the main loop bc camera pos values etc should persist between frames
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); //move the camera 3 forwards so we can see the cube at 0,0,0
    //cameraFront is now declared outside main()
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //world up


    while (!glfwWindowShouldClose(window)) {
        processInput(window, cameraPos, cameraFront, cameraUp); //process/poll input in my own callback

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //sets what colour to wipe the screen to (teal). doesnt draw yet
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //color buffer actually wipes screen with above set color. every frame starts with this to cover previous frame. and depth buffer also tells it to clear the depth buffer
        //since GL_COLOR_BUFFER_BIT and GL_DEPTH_BUFFER_BIT are bit flags, we can combine them with |

        // every frame:
        shader.use();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h); //set w and h to the width and height of the window
        float aspect = (float)w / (float)h;


        glm::mat4 model = glm::mat4(1.0f); //identity
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f)); //rotate over time (0.5 on x and 1 on y so its a tilted axis kinda thing)

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); //make the camera be at right pos and look in right dir

        glm::mat4 projection; //identity
        projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f); //fov = 45, aspect ratio = 800/600, near clip plane = 0.1f (close than this isnt drawn),
                                                                                            // far clip plane 100.0f (further than this isnt drawn)

        int modelLoc = glGetUniformLocation(shader.ID, "model");
        int viewLoc = glGetUniformLocation(shader.ID, "view");
        int projLoc = glGetUniformLocation(shader.ID, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


        glBindVertexArray(VAO); //bind the VAO (replay the recipe)
        glDrawArrays(GL_TRIANGLES, 0, 36); //and draw triangles, starting at vertex 0, using 36 vertices which is my cube (vertex count will have to be updated with changes)


        glfwSwapBuffers(window); //swaps a back buffer ontop which should have game frame drawn to. back buffers reduce tearing etc
        glfwPollEvents(); //check for mouse, keyboard etc. otherwise OS will think its hung
    }

    glfwTerminate(); //tidy shutdown
    return 0;
}
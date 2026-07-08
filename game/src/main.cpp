#include <glad/glad.h> //glad has to be included before glfw3 bc glfw3 will try use glad so glad has to be defined
#include <GLFW/glfw3.h>
#include <iostream>

/*
when we are telling the gpu to do stuff we have 2 things:
- VBO (vertex buffer object). this is the actual data of where vertexes of shapes are
- VAO (vertex array object). this tells the gpu how to interpret/handle the VBO. and then the gpu remembers it so u just need to do it in init
            its like saying 'read the VBO like this' and then we 'bind' it to draw so when we draw the gpu checks the VAO automatically.

when we want coordinates, atleast for now we use NDC - Normalised Device Coordinates.
- as in the name, its normalised, so -1.0 to 1.0
- later on we can map this to real coords
*/

// the 2 below consts are not C++, theyre GLSL (shaders). The R"()" is c++ syntax for a multiline string. you can see from char*
// that these are literally strings as we cant just type GLSL in a .cpp file.
//vertex shaders are applied per vertex
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";
//#version 330 core matches our GL version 3.3 core context
// layout (location = 0) in vec3 aPos; means 'expect an input in slot 0, a vec3, and call it aPos. Slot 0 is the hook the VAO goes into. slot 0 is important
//gl_Position is the 'final position' its the one thing a vertex shader HAS to set. 1.0 is w is perspective. we dont need that yet

//fragment shaders are applied per pixel in the shape. so like the filled color of a shape between the vertexes
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)";
//out vec4 FragColor; is its 1 output, the color
//currently we hardcode it orange (the vec4)


int main() {
    glfwInit(); //starts the library


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //these 3 set up what version we want. core is modern

    GLFWwindow* window = glfwCreateWindow(800, 600, "Phosphene Engine", nullptr, nullptr); //width, height, title, fullscreen-monitor, window_sharing (idk)
    if (window == nullptr) { //above statement returns null if failed, pointer if succeeded.
        std::cout << "Failed to create window\n" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // basically says 'opengl commands from now on will go to this window'

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { //glad goes and fetches the function pointers. it asks glfw where they are // INIT GLAD
                                                                // so glfw context has to be set by now (line 17). also must be before any gl call.
        std::cout << "Failed to init glad\n" << std::endl;
        return -1;
    }


    //the glCreateShader calls return an int, an ID. OpenGL gives us integer handles to objects it handles internally. we always use the ID
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); //create a shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr); //give it the source
    glCompileShader(vertexShader); //compile it

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //same as before but for our fragment shader
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram(); //create a 'Program'
    glAttachShader(shaderProgram, vertexShader); //stages the shaders for attaching linking into program. doesnt actually link it yet
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram); //link the stages shaders above.

    glDeleteShader(vertexShader); //after we link we dont need the individiual shader objects so we can delete them
    glDeleteShader(fragmentShader);


    float vertices[] = { //coordinates in NDC - Normalised Device Coordinates (read top)
        -0.5f, -0.5f, 0.0f, //the 3 vertices of my triangle.
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
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





    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //sets what colour to wipe the screen to (teal). doesnt draw yet
        glClear(GL_COLOR_BUFFER_BIT); //actually wipes screen with above set color. every frame starts with this to cover previous frame.

        // every frame:
        glUseProgram(shaderProgram); //activate the shader program,
        glBindVertexArray(VAO); //bind the VAO (replay the recipe)
        glDrawArrays(GL_TRIANGLES, 0, 3); //and draw triangles, starting at vertex 0, using 3 vertices which is my one triangle


        glfwSwapBuffers(window); //swaps a back buffer ontop which should have game frame drawn to. back buffers reduce tearing etc
        glfwPollEvents(); //check for mouse, keyboard etc. otherwise OS will think its hung
    }

    glfwTerminate(); //tidy shutdown
    return 0;
}
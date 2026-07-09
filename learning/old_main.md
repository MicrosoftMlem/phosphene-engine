
This document has notes and code from the old version of main.cpp when we hardcoded shaders.
Im keeping it because it has good notes.

WRITING SHADERS: (this code block is for a cpp file but its GLSL in the strings)

/* OLD CODE KEPT FOR INFO/LEARNING
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
*/


LINKING SHADERS AND CREATING A PROGRAM:

/*
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
*/

We would then run our program every frame with the statement:

//glUseProgram(shaderProgram)
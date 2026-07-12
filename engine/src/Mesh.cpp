#include "Mesh.h"
#include <vector>
#include <glad/glad.h>

Mesh::Mesh(std::vector<float> vertices) {
    vertexCount = vertices.size() / 8; //.size is total float count, and 8 is bc each vertex is 8 floats (x, y, z, UV x, UV y)

    glGenVertexArrays(1, &VAO); //create the VAO and VBO
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO); //bind them 
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /*
    OpenGL is a state machine, you bind an object (such as the VAO) to make it active, then subsequent calls affect whatever is bound
    so unlike normal c++ instead of passing objects to functions, we bind them and the calls act on the bound one
    */


    //its vertices.data() not vertices bc u have to do .data() to get the actual underlying array (of our vertices)
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW); //upload our mesh vertices into GPU memory
    // GL_STATIC_DRAW tells the gpu that this data wont change often which allows the GPU to optimise

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //as said at the top of the file, this is the VAO recipe itself.
    //position: location 0, 3 floats, stride 8 floats
    //we keep the second arg 3 bc this is the pos attribute, we declare UV attribute below
    //its 8 bc 3 position floats (x/y/z) and 2 UV floats and 3 normals
    //slot 0 (matches the shader), 3 floats per vertex/UV, they're floats, dont normalise them, each vertex and UV is 8*sizeof(float) apart (the stride) (12 bytes)
    // , starting at offset 0
    glEnableVertexAttribArray(0); //turn slot 0 on bc its off by default.

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    //uv: location 1, 2 floats, stride 8, offset 3 floats in
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Mesh::draw() {
    glBindVertexArray(VAO); //bind the VAO (replay the recipe)
    glDrawArrays(GL_TRIANGLES, 0, vertexCount); //and draw triangles, starting at vertex 0, using vertexCount vertices
}
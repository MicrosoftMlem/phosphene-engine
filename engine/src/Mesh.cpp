#include "Mesh.h"
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

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
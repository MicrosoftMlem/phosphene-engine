#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

//we use uniforms bc we send vertex data via the VBO 
//but since matrixes arent per vertex data - its one value shared by every vertex this frame we 
//need a different channel to send it, which is Uniforms
// a uniform is a variable you can access from C++ and is constant for each draw call
// so "Uniform" = same for each vertex in the draw. good bc we want all vertexes to be transformed the same

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

//vColor gone now as we texturing

void main() {
    //order of multiplying is important.
    gl_Position = projection * view * model * vec4(aPos, 1.0); 
    //vec4(aPos, 1.0) is same as: aPos.x, aPos.y, aPos.z, 1.0 as GLSL allows vec4's to be made from a vec3 + another value
    //so transform * vec4(aPos, 1.0) is exactly what is in matrix.md. multiplying a vector by a matrix
    TexCoord = aTexCoord;

    FragPos = vec3(model * vec4(aPos, 1.0)); //world space position
    Normal = mat3(transpose(inverse(model))) * aNormal; //the weird math is making a 'normal matrix'.
    //this line is like Normal = normal_matrix * aNormal;
}   

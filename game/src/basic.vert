#version 330 core
layout (location = 0) in vec3 aPos;

//we use uniforms bc we send vertex data via the VBO 
//but since matrixes arent per vertex data - its one value shared by every vertex this frame we 
//need a different channel to send it, which is Uniforms
// a uniform is a variable you can access from C++ and is constant for each draw call
// so "Uniform" = same for each vertex in the draw. good bc we want all vertexes to be transformed the same

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vColor; //output vColor (will be used by basic.frag)
//this out will automatically go into basic.frags in

void main() {
    //order of multiplying is important.
    gl_Position = projection * view * model * vec4(aPos, 1.0); 
    //vec4(aPos, 1.0) is same as: aPos.x, aPos.y, aPos.z, 1.0 as GLSL allows vec4's to be made from a vec3 + another value
    //so transform * vec4(aPos, 1.0) is exactly what is in matrix.md. multiplying a vector by a matrix
    vColor = aPos + 0.5; //set vColor to the vertex's position, so theyre color changes with their pos
}   

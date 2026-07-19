#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 projection; //ortho
uniform mat4 model; //positions and scales the quad

void main() {
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
}
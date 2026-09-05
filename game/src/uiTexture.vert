#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 model;

void main() {
TexCoord = aPos;
gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
}

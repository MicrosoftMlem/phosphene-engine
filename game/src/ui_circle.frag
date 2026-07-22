#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 color; //circle color (rgba)


void main() {
    if (length(TexCoord - 0.5) > 0.5) discard; //discard anything outside of a circle
    FragColor = color;
}
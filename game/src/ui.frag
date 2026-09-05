#version 330 core

out vec4 FragColor;

uniform vec4 color; //rect color (rgba)

void main() {
     FragColor = color;
}
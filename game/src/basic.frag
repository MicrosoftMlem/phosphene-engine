#version 330 core
out vec4 FragColor;
in vec3 vColor; //take a vec3 as an input which we will use to set the colour
void main() {
    FragColor = vec4(vColor, 1.0);
}
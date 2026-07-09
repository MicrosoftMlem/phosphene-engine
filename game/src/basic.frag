#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D ourTexture;
//sampler2D is a uniform type which is a handle to a texture unit
void main() {
    FragColor = texture(ourTexture, TexCoord); //maps ourTexture to the UV coords from TexCoord
}
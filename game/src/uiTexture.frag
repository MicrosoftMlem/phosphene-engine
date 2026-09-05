#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D u_texture;

void main() {
  vec4 tex = texture(u_texture, TexCoord);
  FragColor = tex;
}
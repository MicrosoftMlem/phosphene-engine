#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D ourTexture;
uniform vec2 textureScale;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

//sampler2D is a uniform type which is a handle to a texture unit
void main() {
    //sample the base color (without lighting)
    vec3 objectColor = texture(ourTexture, TexCoord * textureScale).rgb; //maps ourTexture to the UV coords from TexCoord and scales the UVs
    
    //now diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float ambientStrength = 0.15; //ambient light - a constant light on everything
    vec3 ambient = ambientStrength * lightColor;

    float diff = max(dot(norm, lightDir), 0.0); //how aligning the surface is with the light
    vec3 diffuse = diff * lightColor; //apply brightness / colour

    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); //where the light bounces off the surface
    float spec = pow(max(dot(viewDir, reflectDir), 0.3), 32.0); //pow 32 is shinyness, max(dot(...)) is how directly the reflection points in your eye
    vec3 specular = specularStrength * spec * lightColor;



    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(normalize(Normal)*0.5+0.5, 1.0); //debug to show normals
}
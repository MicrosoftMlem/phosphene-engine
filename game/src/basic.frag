#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_EmissiveMap;

// ActiveState is the state of the emissive map, so not specific to the traffic light. its a vec3 bc the emissive map is based on the rgb channels
uniform vec3 u_ActiveState; // eg (1.0, 0.0, 0.0) for red
uniform float u_BloomIntensity;
uniform bool u_UseEmissive; // if we use emissive map

uniform vec2 textureScale;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform vec3 tint;

//sampler2D is a uniform type which is a handle to a texture unit
void main() {
    //sample the base color (without lighting)
    vec2 scaledUV = TexCoord * textureScale;
    vec4 texSample = texture(u_AlbedoMap, scaledUV); //maps the albedo texture to the UV coords from TexCoord
    vec3 objectColor = texSample.rgb; //actual visuals are only in the rgb channels, not alpha
    
    //lighting calculation
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    //ambient lighting
    float ambientStrength = 0.35; //ambient light - a constant light on everything
    vec3 ambient = ambientStrength * lightColor;
  
    //diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0); //how aligning the surface is with the light
    vec3 diffuse = diff * lightColor; //apply brightness / colour
    
    //specular lighting
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); //where the light bounces off the surface
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 24.0); //pow 32 is shinyness, max(dot(...)) is how directly the reflection points in your eye
    vec3 specular = specularStrength * spec * lightColor;


    vec3 litColor = (ambient + diffuse + specular) * objectColor * tint; //tint multiplies
    
    //compute emissive glow:
    vec3 emissiveOutput = vec3(0.0f);

    if (u_UseEmissive) {
      //read the rgb values from the emissive mask
      vec3 mask = texture(u_EmissiveMap, scaledUV).rgb;
      
      // 'isolated the active light channel (r or g or b)' remember dot turns vec3 into float
      float activeGlowDetail = dot(mask, u_ActiveState);

      //map the active state to the real colours to display
      //WARNING:
      //note that here, its hardcoded what each channel emits. so eg here red = red, green = yellow and blue = green.
      //we need to make 3 uniforms later on, vec3 blueChannelEmissiveColour, etc for red, green and blue channel.
      vec3 emissiveColour = vec3(0.0);
      if (u_ActiveState.r > 0.5) emissiveColour = vec3(1.0, 0.1, 0.1); // red
      else if (u_ActiveState.g > 0.5) emissiveColour = vec3(0.1, 1.0, 0.2); // green
      else if (u_ActiveState.b > 0.5) emissiveColour = vec3(1.0, 0.8, 0.0); // blue
    
      //emissive skips scene lighting so it glows naturally.
      emissiveOutput = activeGlowDetail * emissiveColour * u_BloomIntensity;
    }

    vec3 finalColor = litColor + emissiveOutput; //emissive adds
    FragColor = vec4(finalColor, 1.0);

    //FragColor = vec4(normalize(Normal)*0.5+0.5, 1.0); //debug to show normals
}

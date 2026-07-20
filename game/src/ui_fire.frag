#version 330 core

out vec4 FragColor;

in vec2 TexCoord; //0-1 inside the ui quad

uniform float time;
uniform float pixelSize; //bigger = bigger pixels

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453); //simple hash-based noise generator 
}

float noise(vec2 p) { //generate some noise
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f); //smooth-step interpolation
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    return (mix(mix(a, b, f.x), mix(c, d, f.x), f.y));
}

//hue (1-0) to rgb for the rainbow effect
vec3 hue2rgb(float h) {
    vec3 k = vec3(1.0, 2.0/3.0, 1.0/3.0);
    vec3 p = abs(fract(vec3(h) + k) * 6.0 - 3.0);
    return clamp(p - 1.0, 0.0, 1.0);
}

void main() {
    //pixelate it first
    vec2 uv = floor(TexCoord / pixelSize) * pixelSize;

    //then fire noise scrolling upward-left over time
    float n = noise(uv * 8.0 + vec2(-time * 1.4, -time * 2.0));

    //vertical gradient
    float gradient = TexCoord.y;
    float intensity = n * gradient;

    //rainbow hue cycling with time and pos
    float hue = fract(time * 0.1 + TexCoord.y * 0.5);
    vec3 fireColor = hue2rgb(hue) * intensity * 2.0;

    //then fade out the alpha when intensity is low:
    float alpha = smoothstep(0.1, 0.4, intensity);

    FragColor = vec4(fireColor, alpha);
}
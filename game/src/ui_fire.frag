#version 330 core

out vec4 FragColor;

in vec2 TexCoord; //0-1 inside the ui quad

uniform float time;
uniform float amount; //intensity
uniform float id; //random seed

uniform vec3 color1; //main color
uniform vec3 color2; //hot color

const float PIXEL_SIZE_FAC = 60.0f;


void main() {
    float intensity = min(10.0, amount);
    if (intensity < 0.1) {
        FragColor = vec4(0.0); //clamp it
        return;
    }

    vec2 uv = TexCoord - 0.5; //local uv, centred

    //pixelate
    vec2 floored_uv = floor(uv * PIXEL_SIZE_FAC) / PIXEL_SIZE_FAC;
    vec2 uv_scaled_centred = floored_uv;

    //base distortion
    uv_scaled_centred += uv_scaled_centred * 0.01 * (
        sin(-1.123 * floored_uv.x + 0.2 * time) *
        cos(5.3332 * floored_uv.y + time * 0.931)
    );


    vec2 flame_up_vec = vec2(0.0, mod(4.0 * time, 10000.0) - 5000.0 + mod(1.781 * id, 1000.0));
    float scale_fac = (7.5 + 3.0 / (2.0 + 2.0 * intensity));
    vec2 sv = uv_scaled_centred * scale_fac + flame_up_vec;
    float speed = mod(20.781 * id, 100.0) + 1.0 * sin(time + id) * cos(time * 0.151 + id);
    vec2 sv2 = vec2(0.0);



    //fractal distortion loop
    for (int i = 0; i < 5; i++) {
        float iteration_mod = (mod(float(i), 2.0) > 1.0 ? -1.0 : 1.0);
        sv2 += sv + 0.05 * sv2.yx * iteration_mod + 0.3 * (
            cos(length(sv) * 0.411) + 0.3344 * sin(length(sv)) - 0.23 * cos(length(sv))
        );
        sv += 0.5 * vec2(
            cos(cos(sv2.y) + speed * 0.0812) * sin(3.22 + sv2.x - speed * 0.1531),
            sin(-sv2.x * 1.21222 + 0.113785 * speed) * cos(sv2.y * 0.91213 - 0.13582 * speed)
        );
    }

    //smoke/flame density
    float smoke_res = max(0.0, (
        (length((sv - flame_up_vec) / scale_fac * 5.0) + 0.1 * (length(uv_scaled_centred) - 0.5)) *
        (2.0 / (2.0 + intensity * 0.2))
    ));
    smoke_res = smoke_res + max(0.0, 2.0 - 0.3 * intensity) * max(0.0, 2.0 * (uv_scaled_centred.y - 0.5) * (uv_scaled_centred.y - 0.5));
    // if (abs(uv.x) > 0.4) {
    //     smoke_res += 10.0 * (abs(uv.x) - 0.4);
    // }



    //coloring
    vec3 ret_col = color1;

    if (smoke_res > 1.0) {
        FragColor = vec4(0.0);
    }
    else {
        if (uv.y < 0.12) {
            float y_factor = 0.12 - uv.y;
            ret_col = ret_col * (1.0 - 0.5 * y_factor) + 2.5 * y_factor * color2;
            ret_col += ret_col * (-2.0 + 0.5 * intensity * smoke_res) * y_factor;
        }
        FragColor = vec4(ret_col, 1.0);
    }
}
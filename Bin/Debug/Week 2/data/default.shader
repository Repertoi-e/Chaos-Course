#vertex

#version 330 core
layout (location = 0) in vec3 pos;
 
void main() {
    gl_Position = vec4(pos.xyz, 1.0);
}


#fragment

#version 330 core
in vec4 gl_FragCoord;
 
out vec4 frag_color;

uniform vec2 screen_dim;
 
#define MAX_ITERATIONS 1000

// Kinda stole this gradient 
// https://stackoverflow.com/questions/16500656/which-color-gradient-is-used-to-color-mandelbrot-in-wikipedia
vec3 gradient(float position) {
    vec3 c1 = vec3(0.0, 7.0, 100.0) / 255.0;
    vec3 c2 = vec3(32.0, 107.0, 203.0) / 255.0;
    vec3 c3 = vec3(237.0, 255.0, 255.0) / 255.0;
    vec3 c4 = vec3(255.0, 170.0, 0.0) / 255.0;
    vec3 c5 = vec3(0.0, 2.0, 0.0) / 255.0;
    
    if (position < 0.16) {
        return mix(c1, c2, position / 0.16);
    } else if (position < 0.42) {
        return mix(c2, c3, (position - 0.16) / (0.42 - 0.16));
    } else if (position < 0.6425) {
        return mix(c3, c4, (position - 0.42) / (0.6425 - 0.42));
    } else if (position < 0.8575) {
        return mix(c4, c5, (position - 0.6425) / (0.8575 - 0.6425));
    } else {
        return c5;
    }
}

vec3 mandelbrot() {
    float aspect = screen_dim.x / screen_dim.y;

    float cx = (gl_FragCoord.x / screen_dim.x - 0.7) * 2.5 * aspect;
    float cy = (gl_FragCoord.y / screen_dim.y - 0.5) * 2.5;

    int iter = 0;

    float zx = 0;
    float zy = 0;
    while (iter < MAX_ITERATIONS) {
        float nzx = zx * zx - zy * zy + cx;
        float nzy = 2 * zx * zy + cy;
        zx = nzx;
        zy = nzy;
        if (zx * zx + zy * zy > 4.0) {
            break;
        }
        iter += 1;
    }

    vec3 color;
    if (iter == MAX_ITERATIONS) {
        color = vec3(0, 0, 0);
    } else {
        float smoothed = log2(log2(zx * zx + zy * zy) / 2.0);
        int colorI = int(sqrt(float(iter + 10) - smoothed) * 256.0) % 2048;
        color = gradient(float(colorI) / 2048.0);

        // color = map_color(iter, float(zx * zx), float(zy * zy));
    }
    return color;
}

void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    float rectWidth = screen_dim.x / 10.0;
    float rectHeight = screen_dim.y / 10.0;

    int rectX = int(floor(fragCoord.x / rectWidth));
    int rectY = int(floor(fragCoord.y / rectHeight));

    int rectIndex = rectX + rectY * 10;
    vec3 tint = fract(vec3(sin(float(rectIndex) + 1.0), sin(float(rectIndex) + 2.0), sin(float(rectIndex) + 3.0)));

    vec3 color = mandelbrot();
    if (color == vec3(0.0)) {
        frag_color = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        frag_color = vec4(mix(color, tint, 0.2), 1.0);
    }
}

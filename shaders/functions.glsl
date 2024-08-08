#version 330 core
out vec4 FragColor;

vec4 grayscale(vec4 color) {
    float average = (color.r + color.g + color.b) / 3.0;
    return vec4(average, average, average, color.a);
}
vec4 gradient(vec2 uv, vec4 color1, vec4 color2) {
    return mix(color1, color2, uv.x);
}
vec4 circularGradient(vec2 uv, vec4 color1, vec4 color2) {
    float dist = length(uv - vec2(0.5, 0.5)); // Calculate the distance from the center (0.5, 0.5)
    return mix(color1, color2, dist);
}
vec4 checkerboard(vec2 uv, float size) {
    vec2 grid = floor(uv * size);
    float checker = mod(grid.x + grid.y, 2.0);
    return vec4(checker, checker, checker, 1.0);
}
vec4 stairedGradient(vec2 uv, float size, vec4 color1, vec4 color2) {
    vec2 grid = floor(uv * size);
    float checker = grid.x / grid.y;
    return mix(color1, color2, checker);
}
// Vertical lines function
vec4 verticalLines(vec2 uv, float size, vec4 color1, vec4 color2) {
    vec2 grid = floor(uv * size);
    float checker = mod(grid.x, 2.0);
    return mix(color1, color2, checker);
}

// Horizontal lines function
vec4 horizontalLines(vec2 uv, float size, vec4 color1, vec4 color2) {
    vec2 grid = floor(uv * size);
    float checker = mod(grid.y, 2.0);
    return mix(color1, color2, checker);
}

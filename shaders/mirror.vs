#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec2 u_position; // Mirror position on screen
uniform vec2 u_size;     // Mirror size

out vec2 TexCoords;

void main() {
    // Transform quad to mirror position/size
    vec2 scaledPos = aPos * u_size * 0.5f;
    vec2 finalPos = u_position + scaledPos;
    
    gl_Position = vec4(finalPos, 0.0, 1.0);
    TexCoords = aTexCoord;
}
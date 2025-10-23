#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 glowColor;
uniform float glowIntensity;

void main() {
    // Radial gradient for glow
    vec2 centered = TexCoords * 2.0 - 1.0;
    float dist = length(centered);
    
    // Smooth falloff
    float glow = exp(-dist * 2.0) * glowIntensity;
    
    vec3 finalColor = glowColor * glow;
    
    FragColor = vec4(finalColor, glow);
}
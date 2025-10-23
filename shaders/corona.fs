#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 coronaColor;
uniform float coronaAlpha;
uniform float coronaTemperature;
uniform vec3 viewPos;

void main() {
    // Calculate distance from center for falloff
    vec3 viewDir = normalize(viewPos - FragPos);
    float edge = 1.0 - dot(normalize(Normal), viewDir);
    
    // Exponential falloff towards edge
    float intensity = exp(-edge * 2.0) * coronaAlpha;
    
    // Add some turbulence (simplified)
    float noise = fract(sin(dot(FragPos.xy, vec2(12.9898, 78.233))) * 43758.5453);
    intensity *= (0.8 + noise * 0.4);
    
    vec3 finalColor = coronaColor * intensity;
    
    FragColor = vec4(finalColor, intensity);
}
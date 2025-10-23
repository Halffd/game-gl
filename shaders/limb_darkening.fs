#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform vec3 starColor;
uniform float limbDarkeningU1; // Linear coefficient
uniform float limbDarkeningU2; // Quadratic coefficient
uniform float luminosity;

uniform sampler2D granulationMap;
uniform float granulationTime;

void main() {
    // Calculate limb darkening
    vec3 viewDir = normalize(viewPos - FragPos);
    float mu = dot(normalize(Normal), viewDir);
    mu = clamp(mu, 0.0, 1.0);
    
    // Limb darkening formula: I(μ) = I(1) * [1 - u1(1-μ) - u2(1-μ)²]
    float limbFactor = 1.0 - limbDarkeningU1 * (1.0 - mu) - limbDarkeningU2 * pow(1.0 - mu, 2.0);
    
    // Sample granulation texture with animation
    vec2 animatedCoords = TexCoords + vec2(granulationTime * 0.01, 0.0);
    float granulation = texture(granulationMap, animatedCoords).r;
    granulation = granulation * 0.3 + 0.7; // Reduce contrast
    
    // Combine effects
    vec3 finalColor = starColor * limbFactor * granulation * luminosity;
    
    // HDR output
    FragColor = vec4(finalColor, 1.0);
}
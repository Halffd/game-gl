#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D mirrorTexture;

void main() {
    vec3 color = texture(mirrorTexture, TexCoords).rgb;
    
    // Add mirror frame effect
    vec2 center = vec2(0.5);
    float dist = distance(TexCoords, center);
    float vignette = 1.0 - smoothstep(0.3, 0.5, dist);
    
    // Slight blue tint like real mirrors
    color *= vec3(0.95, 0.98, 1.0);
    color *= vignette;
    
    // Add border
    float border = 0.02;
    if (TexCoords.x < border || TexCoords.x > 1.0 - border ||
        TexCoords.y < border || TexCoords.y > 1.0 - border) {
        color = vec3(0.2, 0.2, 0.2); // Dark border
    }
    
    FragColor = vec4(color, 0.9); // Slightly transparent
}
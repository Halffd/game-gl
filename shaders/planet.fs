#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LocalPos;

// Planet properties
uniform vec3 baseColor1;      // Primary color
uniform vec3 baseColor2;      // Secondary color for gradient
uniform vec3 highlightColor;  // Highlight/specular color
uniform float shininess;      // Controls the sharpness of the highlight
uniform float gradientFactor; // Controls the gradient blend (0.0 to 1.0)

// Light properties
uniform vec3 lightPos;
uniform vec3 viewPos;

// Function to create a simple noise for surface variation
float random(vec3 pos) {
    return fract(sin(dot(pos, vec3(12.9898, 78.233, 45.5432))) * 43758.5453);
}

// Simple noise function for surface detail
float noise(vec3 pos) {
    vec3 i = floor(pos);
    vec3 f = fract(pos);
    
    // Generate random values at the corners of the cube
    float a = random(i);
    float b = random(i + vec3(1.0, 0.0, 0.0));
    float c = random(i + vec3(0.0, 1.0, 0.0));
    float d = random(i + vec3(1.0, 1.0, 0.0));
    float e = random(i + vec3(0.0, 0.0, 1.0));
    float g = random(i + vec3(1.0, 0.0, 1.0));
    float h = random(i + vec3(0.0, 1.0, 1.0));
    float j = random(i + vec3(1.0, 1.0, 1.0));
    
    // Smooth interpolation between the corners
    vec3 u = f * f * (3.0 - 2.0 * f);
    
    // Trilinear interpolation
    return mix(
        mix(mix(a, b, u.x), mix(c, d, u.x), u.y),
        mix(mix(e, g, u.x), mix(h, j, u.x), u.y),
        u.z);
}

// Fractional Brownian motion for more interesting patterns
float fbm(vec3 pos) {
    float total = 0.0;
    float frequency = 1.0;
    float amplitude = 0.5;
    
    for (int i = 0; i < 4; i++) {
        total += noise(pos * frequency) * amplitude;
        frequency *= 2.0;
        amplitude *= 0.5;
    }
    
    return total;
}

void main() {
    // Normalize the normal and view direction
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Calculate lighting (simple directional light)
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Calculate specular component
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    // Generate gradient based on y-coordinate (poles to equator)
    float gradient = (LocalPos.y + 1.0) * 0.5; // Convert from [-1, 1] to [0, 1]
    
    // Add some noise to the gradient for more natural look
    float noiseValue = fbm(LocalPos * 2.0) * 0.2;
    gradient = clamp(gradient + noiseValue, 0.0, 1.0);
    
    // Blend between the two base colors based on the gradient
    vec3 color = mix(baseColor1, baseColor2, gradient * gradientFactor);
    
    // Add specular highlights
    vec3 specular = highlightColor * spec * 0.5;
    
    // Ambient lighting
    vec3 ambient = color * 0.2;
    
    // Combine all lighting components
    color = ambient + color * diff + specular;
    
    // Increase saturation slightly
    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
    color = mix(vec3(luminance), color, 1.2);
    
    FragColor = vec4(color, 1.0);
}

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
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Light uniforms
uniform DirLight dirLight;
uniform PointLight pointLight; // Main point light
uniform SpotLight spotLight;
uniform bool useDirLight;
uniform bool usePointLight;
uniform bool useSpotLight;

// Light brightness adjustment uniforms
uniform float pointLightBrightness = 1.0;
uniform float dirLightBrightness = 1.0;
uniform float spotLightBrightness = 1.0;

// Other uniforms
uniform vec3 viewPos;

// Function to create a simple noise for surface variation
float random(vec3 pos) {
    return fract(sin(dot(pos, vec3(12.9898, 78.233, 45.5432))) * 43758.5453);
}

// Simple noise function for surface detail
float noise(vec3 pos) {
    vec3 i = floor(pos);
    vec3 f = fract(pos);
    
    float a = random(i);
    float b = random(i + vec3(1.0, 0.0, 0.0));
    float c = random(i + vec3(0.0, 1.0, 0.0));
    float d = random(i + vec3(1.0, 1.0, 0.0));
    float e = random(i + vec3(0.0, 0.0, 1.0));
    float g = random(i + vec3(1.0, 0.0, 1.0));
    float h = random(i + vec3(0.0, 1.0, 1.0));
    float j = random(i + vec3(1.0, 1.0, 1.0));
    
    vec3 u = f * f * (3.0 - 2.0 * f);
    
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

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);

void main() {
    // Normalize the normal and view direction
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Generate gradient based on y-coordinate (poles to equator)
    float gradient = (LocalPos.y + 1.0) * 0.5; // Convert from [-1, 1] to [0, 1]
    
    // Add some noise to the gradient for more natural look
    float noiseValue = fbm(LocalPos * 2.0) * 0.2;
    gradient = clamp(gradient + noiseValue, 0.0, 1.0);
    
    // Blend between the two base colors based on the gradient
    vec3 diffuseColor = mix(baseColor1, baseColor2, gradient * gradientFactor);
    vec3 specularColor = highlightColor;

    // Calculate lighting
    vec3 result = vec3(0.0);

    // Directional light
    if(useDirLight) {
        result += CalcDirLight(dirLight, norm, viewDir, diffuseColor, specularColor);
    }

    // Main point light
    if(usePointLight) {
        result += CalcPointLight(pointLight, norm, FragPos, viewDir, diffuseColor, specularColor);
    }

    // Spot light
    if(useSpotLight) {
        result += CalcSpotLight(spotLight, norm, FragPos, viewDir, diffuseColor, specularColor);
    }

    // If no lights are enabled, use a basic ambient light
    if(!useDirLight && !usePointLight && !useSpotLight) {
        result = diffuseColor * 0.3; // Basic ambient
    }
    
    FragColor = vec4(result, 1.0);
}

// Calculates the color when using a directional light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // Combine results
    vec3 ambient = light.ambient * diffuseColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;

    return (ambient + diffuse + specular) * dirLightBrightness;
}

// Calculates the color when using a point light
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // Attenuation
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Combine results
    vec3 ambient = light.ambient * diffuseColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular) * pointLightBrightness;
}

// Calculates the color when using a spot light
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // Attenuation
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Spotlight effect
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Combine results
    vec3 ambient = light.ambient * diffuseColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular) * spotLightBrightness;
}
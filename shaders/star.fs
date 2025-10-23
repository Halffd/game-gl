#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LocalPos;

// Star properties
uniform vec3 starColor;        // Base color of the star
uniform float luminosity;      // Overall brightness/luminosity
uniform float temperature;     // Surface temperature (for color calculation)
uniform float shininess;       // Controls the sharpness of the highlight

// NEW: Blinn-Phong toggle
uniform bool useBlinnPhong = true; // Toggle between Phong and Blinn-Phong

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

// NEW: Unified specular calculation function
float CalculateSpecular(vec3 lightDir, vec3 normal, vec3 viewDir, float shininessValue) {
    if (useBlinnPhong) {
        // Blinn-Phong: use halfway vector
        vec3 halfwayDir = normalize(lightDir + viewDir);
        return pow(max(dot(normal, halfwayDir), 0.0), shininessValue);
    } else {
        // Classic Phong: use reflection vector
        vec3 reflectDir = reflect(-lightDir, normal);
        return pow(max(dot(viewDir, reflectDir), 0.0), shininessValue);
    }
}

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);

void main() {
    // Normalize the normal and view direction
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // For stars, we want a more uniform color with possible radial gradients
    vec3 diffuseColor = starColor;
    vec3 specularColor = vec3(1.0); // Stars are very reflective/light-emitting

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

    // If no lights are enabled, use the star's own luminosity
    if(!useDirLight && !usePointLight && !useSpotLight) {
        result = starColor * luminosity; // Stars emit their own light
    }
    
    FragColor = vec4(result, 1.0);
}

// Calculates the color when using a directional light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading - use the new unified function
    float effectiveShininess = useBlinnPhong ? shininess * 2.5 : shininess; // Blinn-Phong needs higher exponent
    float spec = CalculateSpecular(lightDir, normal, viewDir, effectiveShininess);

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

    // Specular shading - use the new unified function
    float effectiveShininess = useBlinnPhong ? shininess * 2.5 : shininess;
    float spec = CalculateSpecular(lightDir, normal, viewDir, effectiveShininess);

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

    // Specular shading - use the new unified function
    float effectiveShininess = useBlinnPhong ? shininess * 2.5 : shininess;
    float spec = CalculateSpecular(lightDir, normal, viewDir, effectiveShininess);

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
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 FragPos;

// Maximum number of textures
#define MAX_TEXTURES 32

// Material textures
uniform sampler2D texture_diffuse[MAX_TEXTURES];
uniform sampler2D texture_specular[MAX_TEXTURES];
uniform sampler2D texture_normal[MAX_TEXTURES];
uniform sampler2D texture_height[MAX_TEXTURES];

// Counts for each texture type
uniform int diffuseCount;
uniform int specularCount;
uniform int normalCount;
uniform int heightCount;

// Light uniforms
uniform vec3 viewPos;   // Camera position
uniform vec3 lightColor; // General light color

// Control parameters
uniform float edgeThreshold; // Edge detection threshold
uniform int cellShade;       // Enable/disable cell shading (0 = off, 1 = on)
uniform float mixColor;      // Color mixing factor for texture blending

// Structures for different types of lights
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

// Light uniform arrays and single uniforms
#define NR_POINT_LIGHTS 4
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateCellShade(vec3 normal, vec3 lightDir, vec3 lightColor, int shadeLevels);
vec3 hardEdge(vec3 norm, vec3 viewDir);

// Function to get normal from the normal map
vec3 getNormalFromTexture(vec2 texCoords) {
    vec3 normalMap = texture(texture_normal[0], texCoords).rgb;
    normalMap = normalMap * 2.0 - 1.0; // Convert to [-1, 1] range
    return normalize(normalMap);
}

// Function to get height from the height map
float getHeightFromTexture(vec2 texCoords) {
    return texture(texture_height[0], texCoords).r;
}

// Function to calculate ambient, diffuse, and specular lighting
vec3 calculateLighting(vec3 normal, vec3 fragPos, vec3 viewDir) {
    // Start with ambient light
    vec3 result = CalcDirLight(dirLight, normal, viewDir);

    // Add point lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i], normal, fragPos, viewDir);
    }

    // Add spotlight
    result += CalcSpotLight(spotLight, normal, fragPos, viewDir);
    return result;
}

void main() {
    // Accumulate and average all diffuse textures
    vec4 diffuseColor = vec4(0.0);
    for (int i = 0; i < diffuseCount; ++i) {
        diffuseColor += texture(texture_diffuse[i], TexCoords);
    }
    if (diffuseCount > 0) {
        diffuseColor /= float(diffuseCount);
    }

    // Retrieve height for displacement and modify texture coordinates for parallax effect
    float height = getHeightFromTexture(TexCoords);
    vec2 displacedTexCoords = TexCoords + vec2(height * 0.1);

    // Normal mapping - calculate perturbed normal
    vec3 perturbedNormal = getNormalFromTexture(displacedTexCoords);

    // Calculate view direction
    vec3 viewDir = normalize(viewPos - FragPos);

    // Calculate lighting with perturbed normal
    vec3 lighting = calculateLighting(perturbedNormal, FragPos, viewDir);

    // Cell shading, if enabled
    if (cellShade > 0) {
        lighting = CalculateCellShade(perturbedNormal, viewDir, lighting, cellShade);
    }

    // Apply edge detection for outlining
    vec3 edgeFactor = hardEdge(perturbedNormal, viewDir);

    // Final fragment color
    FragColor = vec4(lighting * diffuseColor.rgb * edgeFactor, 1.0);
}

// Function for calculating cell shading
vec3 CalculateCellShade(vec3 normal, vec3 lightDir, vec3 lightColor, int shadeLevels) {
    float diff = max(dot(normalize(normal), normalize(lightDir)), 0.0);
    float level = floor(diff * float(shadeLevels));
    float cellShadeAmount = level / float(shadeLevels);
    return lightColor * cellShadeAmount;
}

// Function for hard edge detection
vec3 hardEdge(vec3 norm, vec3 viewDir) {
    float intensity = dot(norm, viewDir);
    return (intensity < edgeThreshold) ? vec3(0.1) : vec3(1.0);
}

// Directional light calculation
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec;
    vec3 ambient = light.ambient;
    return ambient + diffuse + specular;
}

// Point light calculation
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec;
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 ambient = light.ambient;
    return (ambient + diffuse + specular) * attenuation;
}

// Spotlight calculation
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec;
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    vec3 ambient = light.ambient;
    return (ambient + diffuse + specular) * attenuation * intensity;
}

#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 Color; // Receive color from vertex shader
in vec3 FragPos;

out vec4 FragColor;

#define NR_TEXTURES 32 // Maximum number of textures
struct Material {
    sampler2D texture_diffuse[NR_TEXTURES];
    sampler2D texture_specular[NR_TEXTURES];
    sampler2D texture_normal[NR_TEXTURES];
    sampler2D texture_height[NR_TEXTURES];
    float shininess;
    int diffuseCount; // Count of active diffuse textures
    int specularCount; // Count of active specular textures
    int normalCount; // Count of active normal textures
    int heightCount; // Count of active height textures
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float outerCutOff;
};

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

uniform Material material;

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform DirLight dirLight;

uniform float mixColor;
uniform vec3 viewPos;

// Control parameters
uniform float edgeThreshold; // Threshold for hard edges
uniform int cellShade; // Enable/disable cell shading (0 = off, 1 = on)

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 CalculateCellShade(vec3 normal, vec3 lightDir, vec3 lightColor, int shadeLevels) {
    float diff = max(dot(normalize(normal), normalize(lightDir)), 0.0);
    float level = floor(diff * float(shadeLevels));
    float cellShadeAmount = level / float(shadeLevels);
    return lightColor * cellShadeAmount;
}

vec3 hardEdge(vec3 norm, vec3 viewDir) {
    float intensity = dot(norm, viewDir); // Lighting intensity based on angle
    return (intensity < edgeThreshold) ? vec3(0.1, 0.1, 0.1) : vec3(1.0); // Dark color for edges
}

void main() {
    // Combine textures based on their types
    vec4 finalDiffuse = vec4(0.0);
    vec4 finalSpecular = vec4(0.0);
    vec4 finalNormal = vec4(0.0);
    vec4 finalHeight = vec4(0.0);

    // Accumulate diffuse textures
    for (int i = 0; i < material.diffuseCount; ++i) {
        finalDiffuse += texture(material.texture_diffuse[i], TexCoord);
    }

    // Accumulate specular textures
    for (int i = 0; i < material.specularCount; ++i) {
        finalSpecular += texture(material.texture_specular[i], TexCoord);
    }

    // Accumulate normal textures (if needed for normal mapping)
    for (int i = 0; i < material.normalCount; ++i) {
        finalNormal += texture(material.texture_normal[i], TexCoord);
    }

    // Accumulate height textures (if needed for displacement mapping)
    for (int i = 0; i < material.heightCount; ++i) {
        finalHeight += texture(material.texture_height[i], TexCoord);
    }

    // Lighting calculations
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    result *= finalDiffuse.rgb; // Use accumulated diffuse color for final result

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse[0], TexCoord)); // Use first diffuse texture

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular[0], TexCoord)); // Use first specular texture

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse[0], TexCoord)); // Use first diffuse texture

    if (cellShade > 0) {
        diffuse = CalculateCellShade(normal, lightDir, light.diffuse, cellShade);
    }

    vec3 edgeFactor = hardEdge(normal, viewDir);
    return (ambient + (diffuse * edgeFactor) + (specular * edgeFactor));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse[0], TexCoord)); // Use first diffuse texture

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular[0], TexCoord)); // Use first specular texture

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse[0], TexCoord)); // Use first diffuse texture

    if (cellShade > 0) {
        diffuse = CalculateCellShade(normal, lightDir, light.diffuse, cellShade);
    }

    vec3 edgeFactor = hardEdge(normal, viewDir);
    return (ambient + (diffuse * edgeFactor) + (specular * edgeFactor)) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse[0], TexCoord)); // Use first diffuse texture

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular[0], TexCoord)); // Use first specular texture

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse[0], TexCoord)); // Use first diffuse texture

    if (cellShade > 0) {
        diffuse = CalculateCellShade(normal, lightDir, light.diffuse, cellShade);
    }

    return (ambient + diffuse + specular) * attenuation * intensity;
}
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

// Material maps
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
uniform sampler2D texture_detail1;
uniform sampler2D texture_scatter1;

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
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform bool useDirLight;
uniform bool usePointLight;
uniform bool useSpotLight;

// Other uniforms
uniform vec3 viewPos;
uniform bool useNormalMap;
uniform bool useSpecularMap;
uniform bool useDetailMap;
uniform bool useScatterMap;
uniform float shininess;

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);

void main() {
    // Sample the diffuse texture
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    
    // If the texture is completely transparent, discard the fragment
    if(texColor.a < 0.1)
        discard;
    
    // Get diffuse color
    vec3 diffuseColor = texColor.rgb;
    
    // Apply detail map if available
    if(useDetailMap) {
        vec3 detailColor = texture(texture_detail1, TexCoords * 5.0).rgb; // Scale UVs for detail
        diffuseColor = mix(diffuseColor, detailColor, 0.3); // Blend with 30% detail
    }
    
    // Get specular color
    vec3 specularColor = vec3(0.5);
    if(useSpecularMap) {
        specularColor = texture(texture_specular1, TexCoords).rgb;
    }
    
    // Get normal from normal map or use the interpolated normal
    vec3 norm;
    if(useNormalMap) {
        norm = texture(texture_normal1, TexCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0); // Transform from [0,1] to [-1,1]
    } else {
        norm = normalize(Normal);
    }
    
    // Calculate view direction
    vec3 viewDir;
    if(useNormalMap) {
        viewDir = normalize(TangentViewPos - TangentFragPos);
    } else {
        viewDir = normalize(viewPos - FragPos);
    }
    
    // Calculate scatter effect if available
    float scatter = 0.0;
    if(useScatterMap) {
        scatter = texture(texture_scatter1, TexCoords).r;
    }
    
    // Calculate lighting
    vec3 result = vec3(0.0);
    
    // Directional light
    if(useDirLight) {
        result += CalcDirLight(dirLight, norm, viewDir, diffuseColor, specularColor);
    }
    
    // Point light
    if(usePointLight) {
        result += CalcPointLight(pointLight, norm, FragPos, viewDir, diffuseColor, specularColor);
    }
    
    // Spot light
    if(useSpotLight) {
        result += CalcSpotLight(spotLight, norm, FragPos, viewDir, diffuseColor, specularColor);
    }
    
    // Add scatter effect
    if(useScatterMap) {
        result += diffuseColor * scatter * 0.5;
    }
    
    // If no lights are enabled, use a basic ambient light
    if(!useDirLight && !usePointLight && !useSpotLight) {
        result = diffuseColor * 0.3; // Basic ambient
    }
    
    FragColor = vec4(result, texColor.a);
}

// Calculates the color when using a directional light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir;
    if(useNormalMap) {
        lightDir = normalize(-TangentLightPos);
    } else {
        lightDir = normalize(-light.direction);
    }
    
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    // Combine results
    vec3 ambient = light.ambient * diffuseColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;
    
    return (ambient + diffuse + specular);
}

// Calculates the color when using a point light
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir;
    float distance;
    
    if(useNormalMap) {
        lightDir = normalize(TangentLightPos - TangentFragPos);
        distance = length(TangentLightPos - TangentFragPos);
    } else {
        lightDir = normalize(light.position - fragPos);
        distance = length(light.position - fragPos);
    }
    
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
    
    return (ambient + diffuse + specular);
}

// Calculates the color when using a spot light
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir;
    float distance;
    
    if(useNormalMap) {
        lightDir = normalize(TangentLightPos - TangentFragPos);
        distance = length(TangentLightPos - TangentFragPos);
    } else {
        lightDir = normalize(light.position - fragPos);
        distance = length(light.position - fragPos);
    }
    
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    // Attenuation
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // Spotlight intensity
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
    
    return (ambient + diffuse + specular);
} 
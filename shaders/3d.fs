#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec3 TangentSpotLightPos;
in vec3 TangentSpotLightDir;

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
uniform PointLight pointLight; // Main point light
uniform SpotLight spotLight;
uniform bool useDirLight;
uniform bool usePointLight;
uniform bool useSpotLight;

// Multiple point lights
#define MAX_POINT_LIGHTS 20
uniform PointLight randomPointLights[MAX_POINT_LIGHTS];
uniform int numRandomPointLights;
uniform bool useRandomPointLights;

// Light brightness adjustment uniforms
uniform float pointLightBrightness = 0.2; // Default: 20% of original (80% reduction)
uniform float dirLightBrightness = 1.4;   // Default: 140% of original (40% increase)
uniform float spotLightBrightness = 1.8;  // Default: 180% of original (80% increase)

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
float near = 0.1;
float far  = 100.0;
uniform vec3 fogColor = vec3(0.7, 0.7, 0.7); // light gray fog
uniform float fogStart = 3.0;               // fog starts here
uniform float fogEnd = 35.0;                 // fully fogged here

// Checkerboard normal function
vec3 calculateCheckerboardNormal(vec2 position, float scale, float height) {
    // Create a checkerboard pattern
    float x = floor(position.x * scale);
    float z = floor(position.y * scale);
    bool isEven = mod(x + z, 2.0) < 1.0;

    // Create normal vector - pointing up for even squares, angled for odd squares
    vec3 normal = vec3(0.0, 1.0, 0.0);
    if (!isEven) {
        // Calculate normal based on position within the checker
        float fx = fract(position.x * scale);
        float fz = fract(position.y * scale);

        // Create a bump in the center of each odd square
        float dx = 0.5 - fx;
        float dz = 0.5 - fz;
        float dist = sqrt(dx * dx + dz * dz);

        if (dist < 0.4) {
            // Create a dome-like normal
            normal = normalize(vec3(dx * height, 0.5, dz * height));
        }
    }

    return normalize(normal);
}
// Recover eye‑space Z from non‑linear depth buffer
float LinearizeDepth(float d)
{
    // d = depth in non-linear space
    float z_ndc = d * 2.0 - 1.0;
    return (2.0 * near * far)
         / (far + near - z_ndc * (far - near));
}
vec3 ApplyFog(vec3 color, float depth)
{
    float fogFactor = clamp((fogEnd - depth) / (fogEnd - fogStart), 0.0, 1.0);
    return mix(fogColor, color, fogFactor);
}
void depthTest()
{
    // get eye-space Z, then normalize to [0,1] for visualization
    float linearZ = LinearizeDepth(gl_FragCoord.z);
    float normalized = linearZ / far;
    // output normalized Z to the fragment shader
    FragColor = vec4(vec3(normalized), 1.0);
}

void main()
{
    // depthTest(); // call depthTest function
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

    // Get normal from normal map, use checkerboard for ground, or use the interpolated normal
    vec3 norm;
    if(useNormalMap) {
        norm = texture(texture_normal1, TexCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0); // Transform from [0,1] to [-1,1]
    } else {
        // Check if this is the ground plane (y-coordinate close to 0)
        if (abs(FragPos.y) < 0.1) {
            // Apply checkerboard normal for ground
            norm = calculateCheckerboardNormal(FragPos.xz, 1.0, 0.255); // 255 lower (0.255 instead of 0.5)
            // Transform to world space
            norm = normalize(mat3(transpose(inverse(mat3(1.0)))) * norm);
        } else {
            norm = normalize(Normal);
        }
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

    // Main point light
    if(usePointLight) {
        result += CalcPointLight(pointLight, norm, FragPos, viewDir, diffuseColor, specularColor);
    }

    // Random point lights
    if(useRandomPointLights) {
        for(int i = 0; i < numRandomPointLights && i < MAX_POINT_LIGHTS; i++) {
            result += CalcPointLight(randomPointLights[i], norm, FragPos, viewDir, diffuseColor, specularColor);
        }
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
    if(!useDirLight && !usePointLight && !useSpotLight && !useRandomPointLights) {
        result = diffuseColor * 0.3; // Basic ambient
    }
    float linearZ = LinearizeDepth(gl_FragCoord.z);
    float fade = clamp(1.0 - linearZ / 8.0, 0.0, 1.0); // fades beyond 50 units
    result *= fade;

    // Apply fog
    result = ApplyFog(result, linearZ);
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
    // Square the diffuse factor to increase intensity
    diff = diff * diff;
    
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // Square the specular factor to increase intensity
    spec = spec * spec;
    
    // Combine results
    vec3 ambient = light.ambient * diffuseColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;
    
    // Apply directional light brightness adjustment using uniform
    return (ambient + diffuse + specular) * dirLightBrightness;
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
    
    // Improved attenuation with distance-based falloff
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // Apply a distance-based cutoff for more realistic falloff
    float distanceFactor = 1.0 - smoothstep(20.0, 40.0, distance); // Gradual falloff between 20 and 40 units
    attenuation *= distanceFactor;
    
    // Combine results
    vec3 ambient = light.ambient * diffuseColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    // Apply point light brightness adjustment using uniform
    return (ambient + diffuse + specular) * pointLightBrightness;
}

// Calculates the color when using a spot light
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir;
    float distance;
    vec3 spotDirection;
    
    if(useNormalMap) {
        // For normal mapping, use the tangent space spotlight position and direction
        lightDir = normalize(TangentSpotLightPos - TangentFragPos);
        distance = length(TangentSpotLightPos - TangentFragPos);
        spotDirection = normalize(TangentSpotLightDir);
    } else {
        // Use the world space spotlight position and direction
        lightDir = normalize(light.position - fragPos);
        distance = length(light.position - fragPos);
        spotDirection = normalize(light.direction);
    }
    
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    // Attenuation
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // Distance-based falloff
    float distanceFactor = 1.0 - smoothstep(15.0, 30.0, distance);
    attenuation *= distanceFactor;
    
    // Spotlight cone effect - use the exact direction from the uniform
    float theta = dot(lightDir, -spotDirection);
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // Smoother falloff
    intensity = smoothstep(0.0, 1.0, intensity);
    
    // Combine results
    vec3 ambient = light.ambient * diffuseColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    // Apply brightness adjustment
    return (ambient + diffuse + specular) * spotLightBrightness;
} 
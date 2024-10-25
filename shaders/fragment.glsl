#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 Color; // Receive color from vertex shader
in vec3 FragPos;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};

struct SpotLight {
    vec3  position;
    vec3  direction;
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

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixColor;
uniform float steps;

uniform vec3 objectColor;
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
    // Calculate the dot product for lighting
    float intensity = dot(norm, viewDir); // Lighting intensity based on angle

    // Check if the intensity is below the threshold
    if (intensity < edgeThreshold) {
        // Apply a dark color for the outline
        return vec3(0.1, 0.1, 0.1); // Very dark color for edges
    } else {
        return vec3(1.0); // Standard color
    }
}
void main() {
    vec4 tex1 = texture(texture1, TexCoord);
    vec4 tex2 = texture(texture2, TexCoord);
    vec4 tex = tex1;
    float mixFactor;

    vec4 colors;
    if (mixColor < 0) {
        mixFactor = 0.3;
    } else {
        mixFactor = mixColor;
    }
    if (tex1 != vec4(0.0) && tex2 != vec4(0.0)) {
        tex = mix(tex1, tex2, mixFactor);
    }
    if (Color != vec3(0.0) && tex == vec4(0.0)) {
        colors = vec4(Color, 1.0); // Use color
    } else if (Color != vec3(0.0)) {
        colors = mix(tex, vec4(Color, 1.0), mixFactor);
    } else {
        colors = tex;
    }

    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // Phase 2: Point lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++){
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
    // Phase 3: Spot light
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    result *= colors.rgb;
    //vec3 edge = hardEdge(norm, viewDir);
    //if(edge == vec3(0.1)){
        //result = vec3(0.1);
    //}
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    // Ambient lighting
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    // Apply cell shading
    if (cellShade > 0) {
        diffuse = CalculateCellShade(normal, lightDir, light.diffuse, cellShade); // 3 shading levels
    }

    // Apply hard edges
    vec3 edgeFactor = hardEdge(normal, viewDir);
    return (ambient + (diffuse * edgeFactor) + (specular * edgeFactor));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Ambient lighting
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    // Apply cell shading
    if (cellShade > 0) {
        diffuse = CalculateCellShade(normal, lightDir, light.diffuse, cellShade); // 3 shading levels
    }

    // Apply hard edges
    vec3 edgeFactor = hardEdge(normal, viewDir);

    return (ambient + (diffuse * edgeFactor) + (specular * edgeFactor)) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Ambient lighting
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    // Apply cell shading
    if (cellShade > 0) {
        diffuse = CalculateCellShade(normal, lightDir, light.diffuse, cellShade); // 3 shading levels
    }

    // Apply hard edges
    //vec3 edgeFactor = hardEdge(normal, viewDir);

    return (ambient + (diffuse) + (specular)) * attenuation * intensity;
}
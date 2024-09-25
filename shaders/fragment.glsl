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
uniform float cellFactor;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 CalculateCellShade(vec3 normal, vec3 lightDir, vec3 lightColor, int shadeLevels) {
    // Calculate the dot product and clamp it to the range [0, 1]
    float diff = max(dot(normalize(normal), normalize(lightDir)), 0.0);

    // Determine the shading level
    float level = floor(diff * float(shadeLevels)); // Number of shading levels
    float cellShadeAmount = level / float(shadeLevels); // Calculate the amount of shading

    // Return the final cell-shaded color
    return lightColor * cellShadeAmount;
}
void main()
{
    vec4 tex1 = texture(texture1, TexCoord);
    vec4 tex2 = texture(texture2, TexCoord);
    vec4 tex = tex1;
    float mixFactor;

    vec4 colors;
    if(mixColor < 0){
        mixFactor = 0.3;
    } else {
        mixFactor = mixColor;
    }
    if(tex1 != vec4(0.0) && tex2 != vec4(0.0)){
        tex = mix(tex1, tex2, mixFactor);
    }
    if(Color != vec3(0.0) && tex == vec4(0.0)){
        colors = vec4(Color, 1.0); // Use color
    } else if(Color != vec3(0.0)){
        colors = mix(tex, vec4(Color, 1.0), mixFactor);
    } else {
        colors = tex;
    }
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    // phase 3: Spot light
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    result *= colors.rgb;
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    return (ambient + diffuse + specular);
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}


// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Material properties
uniform vec3 objectColor;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;
uniform bool hasTexture;
uniform sampler2D texture_diffuse;

// Light properties
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main() {
    // Ambient lighting
    vec3 ambientLight = ambient * lightColor;
    
    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseLight = diff * diffuse * lightColor;
    
    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specularLight = spec * specular * lightColor;
    
    // Combine lighting with object color
    vec3 result;
    
    if (hasTexture) {
        // Use texture
        vec4 texColor = texture(texture_diffuse, TexCoords);
        result = (ambientLight + diffuseLight + specularLight) * texColor.rgb;
        FragColor = vec4(result, texColor.a);
    } else {
        // Use object color
        result = (ambientLight + diffuseLight + specularLight) * objectColor;
        FragColor = vec4(result, 1.0);
    }
} 
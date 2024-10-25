#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1; // Diffuse texture
uniform sampler2D texture_specular1; // Specular texture

uniform vec3 lightPos; // Position of the light
uniform vec3 viewPos;  // Camera position
uniform vec3 lightColor; // Color of the light
uniform float shininess; // Shininess factor for specular

void main()
{
    // Diffuse color
    vec4 diffuse = texture(texture_diffuse1, TexCoords);

    // Normalize the normal vector
    vec3 norm = normalize(Normal);
    
    // Light direction
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Specular color from the texture
    float specularStrength = texture(texture_specular1, TexCoords).r; // Assuming specular texture is grayscale
    if(specularStrength <= 0.0 || specularStrength > 1.0){
        specularStrength = 0.5; // Default value for specular strength
    }
    
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Reflect direction
    vec3 reflectDir = reflect(-lightDir, norm);
    
    // Specular lighting calculation
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    // Final color calculation
    vec3 ambient = 0.2 * diffuse.rgb; // Increased ambient light
    vec3 result = (ambient + diff * lightColor * diffuse.rgb + specularStrength * spec * lightColor) * diffuse.rgb;

    FragColor = vec4(result, 1.0); // Corrected variable name
}
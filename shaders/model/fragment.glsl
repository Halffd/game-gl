#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1; // Diffuse texture
uniform sampler2D texture_specular1; // Specular texture

// Point light
uniform vec3 pointLightPos; // Position of the point light
uniform vec3 pointLightColor; // Color of the point light

// Spotlight
uniform vec3 spotLightPos; // Position of the spotlight
uniform vec3 spotLightColor; // Color of the spotlight
uniform float spotLightCutoff; // Cutoff angle for the spotlight
uniform float spotLightOuterCutoff; // Outer cutoff angle for the spotlight

uniform vec3 viewPos;  // Camera position
uniform float shininess; // Shininess factor for specular

void main()
{
    // Diffuse color
    vec4 diffuse = texture(texture_diffuse1, TexCoords);
    vec3 norm = normalize(Normal); // Normalize the normal vector

    // ----- Point Light Calculations -----
    vec3 lightDir = normalize(pointLightPos - FragPos);
    float pointDiff = max(dot(norm, lightDir), 0.0);

    // Specular color from the texture
    float specularStrength = texture(texture_specular1, TexCoords).r;
    if (specularStrength <= 0.0 || specularStrength > 1.0) {
        specularStrength = 0.5; // Default value for specular strength
    }

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float pointSpec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // Final color calculation for point light
    vec3 pointLightResult = (0.1 * diffuse.rgb) + // Lower ambient light
                            (pointDiff * pointLightColor * diffuse.rgb) + 
                            (specularStrength * pointSpec * pointLightColor);

    // ----- Spotlight Calculations -----
    vec3 toSpotLight = normalize(spotLightPos - FragPos);
    
    // Calculate the angle between the spotlight direction and the fragment position
    float theta = dot(toSpotLight, normalize(viewPos - spotLightPos));
    float epsilon = spotLightCutoff - spotLightOuterCutoff;

    // Calculate intensity only if within the cutoff range
    float intensity = 0.0;
    if (theta > spotLightOuterCutoff) {
        intensity = clamp((theta - spotLightOuterCutoff) / epsilon, 0.0, 1.0);
    }
    
    // Calculate diffuse and specular components for the spotlight
    float spotDiff = max(dot(norm, toSpotLight), 0.0) * intensity;
    float spotSpec = pow(max(dot(viewDir, reflect(-toSpotLight, norm)), 0.0), shininess) * intensity;

    // Final color calculation for spotlight
    vec3 spotLightResult = (0.1 * diffuse.rgb) + // Lower ambient light for spotlight
                           (spotDiff * spotLightColor * diffuse.rgb) + 
                           (specularStrength * spotSpec * spotLightColor);

    // Combine results
    vec3 finalColor = pointLightResult + spotLightResult;

    FragColor = vec4(finalColor, 1.0); // Final fragment color
}
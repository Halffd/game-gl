#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;  // Texture coordinates from vertex shader

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform sampler2D reflectionMap;  // Reflection map to control reflection intensity
uniform bool useReflectionMap;    // Flag to enable/disable reflection map
uniform float reflectivity;       // Base reflectivity factor
uniform sampler2D diffuseTexture; // Diffuse texture for the material
uniform bool useDiffuseTexture;   // Flag to enable/disable diffuse texture

void main()
{
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));

    // Sample from the skybox using the reflection vector
    vec3 reflectColor = texture(skybox, R).rgb;

    // If using reflection map, get the reflection intensity from the texture
    float reflectionIntensity = 1.0;
    if (useReflectionMap) {
        reflectionIntensity = texture(reflectionMap, TexCoords).r;
    } else {
        reflectionIntensity = reflectivity; // Use base reflectivity
    }

    // Calculate Fresnel effect for more realistic reflections
    float fresnel = pow(1.0 - max(dot(normalize(Normal), -I), 0.0), 5.0);
    float reflectionIntensityWithFresnel = reflectionIntensity * fresnel;

    // Sample a base color (could be from another texture or uniform)
    vec3 baseColor;
    if (useDiffuseTexture) {
        baseColor = texture(diffuseTexture, TexCoords).rgb;
    } else {
        baseColor = vec3(0.5, 0.5, 0.5); // Default gray color
    }

    vec3 finalColor = mix(baseColor, reflectColor, reflectionIntensityWithFresnel);

    FragColor = vec4(finalColor, 1.0);
}
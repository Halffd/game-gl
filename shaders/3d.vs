#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;
out vec3 TangentSpotLightPos;
out vec3 TangentSpotLightDir;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Light positions for tangent space calculations
uniform vec3 lightPos; // Point light position
uniform vec3 viewPos;  // Camera position
uniform bool useNormalMap;

// Add these uniforms
uniform vec3 spotLightPos;  // Spotlight position
uniform vec3 spotLightDir;  // Spotlight direction

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    
    // Calculate TBN matrix for normal mapping
    if (useNormalMap && length(aTangent) > 0.0) {
        vec3 T = normalize(mat3(model) * aTangent);
        vec3 N = normalize(mat3(model) * aNormal);
        // Re-orthogonalize T with respect to N
        T = normalize(T - dot(T, N) * N);
        vec3 B = cross(N, T);
        
        mat3 TBN = transpose(mat3(T, B, N));
        TangentLightPos = TBN * lightPos;
        TangentViewPos = TBN * viewPos;
        TangentFragPos = TBN * FragPos;
        TangentSpotLightPos = TBN * spotLightPos;
        TangentSpotLightDir = TBN * spotLightDir;
    }
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
} 
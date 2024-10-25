#version 330 core

layout (location = 0) in vec3 aPos;         // Vertex position
layout (location = 1) in vec3 aNormal;      // Vertex normal
layout (location = 2) in vec2 aTexCoords;   // Texture coordinates
layout (location = 3) in vec3 aTangent;     // Vertex tangent
layout (location = 4) in vec3 aBitangent;   // Vertex bitangent
layout (location = 5) in ivec4 m_BoneIDs;   // Bone IDs
layout (location = 6) in vec4 m_Weights;    // Bone weights

out vec2 TexCoords;
out vec3 Tangent;           // Pass tangent to fragment shader
out vec3 Bitangent;         // Pass bitangent to fragment shader
out vec3 Normal;            // Pass normal to fragment shader
out vec3 FragPos;           // Pass fragment position to fragment shader
out ivec4 BoneIDs;          // Pass bone IDs to fragment shader
out vec4 Weights;           // Pass bone weights to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;    
    Tangent = aTangent;
    Bitangent = aBitangent;
    Normal = aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0)); // Transform position to world space
    BoneIDs = m_BoneIDs;
    Weights = m_Weights;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
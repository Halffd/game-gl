#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec3 aColor; // New color attribute

out vec2 TexCoord;
out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;
out vec3 Color; // Pass color to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoord = aUV;
    Normal = aNormal;
    Tangent = aTangent;
    Bitangent = aBitangent;
      if (gl_VertexAttribArrayEnabled(5)) {
        Color = aColor;
    } else {
        Color = vec3(1.0, 1.0, 1.0); // Default to white color if no color attribute is provided
    }
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
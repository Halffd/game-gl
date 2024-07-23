#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 Color; // Receive color from vertex shader

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec4 mixColor;

void main()
{
    if(Color != vec3(0.0)){
        FragColor = vec4(Color, 1.0); // Use color
    } else {
        FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);

    }

}
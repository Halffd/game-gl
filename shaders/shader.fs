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
    vec4 tex1 = texture(texture1, TexCoord);
    vec4 tex2 = texture(texture2, TexCoord);
    vec4 tex = tex1;
    if(tex1 != vec4(0.0) && tex2 != vec4(0.0)){
        tex = mix(tex1, tex2, 0.2);
    }
    if(Color != vec3(0.0) && tex == vec4(0.0)){
        FragColor = vec4(Color, 1.0); // Use color
    } else if(Color != vec3(0.0)){
        FragColor = mix(tex, vec4(Color, 1.0), 0.5);
    } else {
        FragColor = tex;
    }
}
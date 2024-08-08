#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 Color; // Receive color from vertex shader

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    FragColor = vec4(lightColor * objectColor, 1.0);
    return;
    vec4 tex1 = texture(texture1, TexCoord);
    vec4 tex2 = texture(texture2, TexCoord);
    vec4 tex = tex1;
    float mixFactor;
    if(mixColor <= 0){
        mixFactor = 0.3;
    } else {
        mixFactor = mixColor;
    }
    if(tex1 != vec4(0.0) && tex2 != vec4(0.0)){
        tex = mix(tex1, tex2, mixFactor);
    }
    if(Color != vec3(0.0) && tex == vec4(0.0)){
        FragColor = vec4(Color, 1.0); // Use color
    } else if(Color != vec3(0.0)){
        FragColor = mix(tex, vec4(Color, 1.0), mixFactor);
    } else {
        FragColor = tex;
    }
}
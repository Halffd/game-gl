#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 Color; // Receive color from vertex shader
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixColor;
uniform float darkness;
uniform float specularStrength;
uniform int factor;
uniform float steps;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
//    float steps = 7.2; // Adjust this value to control the number of shading levels
    float cellShade = clamp(floor(diff * steps) / (steps - 1.0), 0.0, 1.0);
    vec3 ambient = lightColor * darkness;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), factor);
    vec3 specular = specularStrength * spec * lightColor;

    vec4 tex1 = texture(texture1, TexCoord);
    vec4 tex2 = texture(texture2, TexCoord);
    vec4 tex = tex1;
    float mixFactor;

    vec4 colors;
    if(mixColor <= 0){
        mixFactor = 0.3;
    } else {
        mixFactor = mixColor;
    }
    if(tex1 != vec4(0.0) && tex2 != vec4(0.0)){
        tex = mix(tex1, tex2, mixFactor);
    }
    if(Color != vec3(0.0) && tex == vec4(0.0)){
        colors = vec4(Color, 1.0); // Use color
    } else if(Color != vec3(0.0)){
        colors = mix(tex, vec4(Color, 1.0), mixFactor);
    } else {
        colors = tex;
    }

    vec3 result = (ambient + cellShade + specular) * colors.rgb;
    FragColor = vec4(result, 1.0);
}
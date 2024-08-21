#version 330 core
out vec4 FragColor;
in vec3 Color;
in vec2 TexCoord;

uniform vec3 lightColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    FragColor = vec4(lightColor, 1.0); // set all 4 vector values to 1.0

    vec4 tex1 = texture(texture1, TexCoord);
    vec4 tex2 = texture(texture2, TexCoord);
    vec4 tex = tex1;
    FragColor = tex;
}
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrScene;
uniform sampler2D bloomBlur;
uniform float exposure;
uniform float bloomStrength;

void main() {
    vec3 hdrColor = texture(hdrScene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    
    // Additive blending
    vec3 result = hdrColor + bloomColor * bloomStrength;
    
    // Tone mapping (Reinhard)
    result = result / (result + vec3(1.0));
    
    // Exposure adjustment
    result = vec3(1.0) - exp(-result * exposure);
    
    // Gamma correction
    result = pow(result, vec3(1.0 / 2.2));
    
    FragColor = vec4(result, 1.0);
}
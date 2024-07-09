#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec4 mixColor;

void main()
{
    vec4 color1 = texture(texture1, TexCoord);
    vec4 color2 = texture(texture2, TexCoord);
    
    // Calculate the average of the RGB channels for each color
    float average1 = (color1.r + color1.g + color1.b) / 3.0;
    float average2 = (color2.r + color2.g + color2.b) / 3.0;
    
    // Mix the two grayscale values based on the mix factor
    float mixFactor = 0.2; // Adjust the mix factor as desired
    float mixedAverage = mix(average1, average2, mixFactor);
    
    // Set the final color to grayscale
    FragColor = vec4(mixedAverage, mixedAverage, mixedAverage, 1.0);
}
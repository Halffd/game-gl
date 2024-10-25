#version 330 core
out vec4 FragColor;

uniform vec4 objectColor;  // Color of the object
uniform vec4 shadowColor;  // Color of the shadow

void main()
{
    // Calculate the distance from the fragment to the border
    float distance = min(gl_FragCoord.x, min(gl_FragCoord.y, min(gl_FragCoord.z, min(abs(gl_FragCoord.x - 800.0), abs(gl_FragCoord.y - 600.0)))));

    // Calculate the shadow factor based on the distance
    float shadowFactor = smoothstep(0.0, 10.0, distance);

    // Blend the object color with the shadow color based on the shadow factor
    FragColor = mix(objectColor, shadowColor, shadowFactor);
}
#version 330 core
out vec4 FragColor;

vec4 objectColor = vec4(0.3, 0.8, 0.5, 1.0);  // Color of the object
vec4 shadowColor = vec4(0.3, 0.2, 0.1, 1.0);  // Color of the shadow
vec2 windowSize = vec2();   // Size of the window

void main()
{
    // Calculate the distance from the fragment to the border
    float distance = min(gl_FragCoord.x, min(gl_FragCoord.y, min(windowSize.x - gl_FragCoord.x, windowSize.y - gl_FragCoord.y)));

    // Calculate the shadow factor based on the distance
    float shadowFactor = smoothstep(0.0, 10.0, distance);

    // Blend the object color with the shadow color based on the shadow factor
    FragColor = mix(objectColor, shadowColor, shadowFactor);
}
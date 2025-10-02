#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float shininess;
uniform int visualMode; // 0=spatial, 1=angular, 2=combined, 3=heatmap
uniform vec3 objectColor;
uniform bool blinn; // Toggle between Phong and Blinn-Phong

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    float ndoth = max(dot(norm, halfwayDir), 0.0);
    float intensity = pow(ndoth, shininess);
    
    // SPATIAL PARTIALS using screen-space derivatives! 🧠
    float dI_dx = dFdx(intensity);
    float dI_dy = dFdy(intensity);
    float spatialGradMag = sqrt(dI_dx*dI_dx + dI_dy*dI_dy);
    
    // ANGULAR PARTIALS (approximated through neighboring samples)
    float epsilon = 0.01;
    
    // Approximate ∂I/∂θ by perturbing light direction
    vec3 lightDir_theta = normalize(vec3(
        lightDir.x + epsilon * cos(atan(lightDir.y, lightDir.x)),
        lightDir.y + epsilon * sin(atan(lightDir.y, lightDir.x)),
        lightDir.z
    ));
    vec3 halfwayDir_theta = normalize(lightDir_theta + viewDir);
    float intensity_theta = pow(max(dot(norm, halfwayDir_theta), 0.0), shininess);
    float dI_dtheta = (intensity_theta - intensity) / epsilon;
    
    // Approximate ∂I/∂φ 
    vec3 lightDir_phi = normalize(vec3(
        lightDir.x,
        lightDir.y,
        lightDir.z + epsilon
    ));
    vec3 halfwayDir_phi = normalize(lightDir_phi + viewDir);
    float intensity_phi = pow(max(dot(norm, halfwayDir_phi), 0.0), shininess);
    float dI_dphi = (intensity_phi - intensity) / epsilon;
    
    float angularGradMag = sqrt(dI_dtheta*dI_dtheta + dI_dphi*dI_dphi);
    
    // SHININESS PARTIAL ∂I/∂n (this one's analytically exact!)
    float dI_dn = (ndoth > 0.0) ? intensity * log(ndoth) : 0.0;
    
    // Total gradient magnitude
    float totalGradMag = sqrt(spatialGradMag*spatialGradMag + 
                             angularGradMag*angularGradMag + 
                             dI_dn*dI_dn);
    
    vec3 color = vec3(0.0);
    
    if(visualMode == 0) // SPATIAL GRADIENTS
    {
        // Red = ∂I/∂x, Green = ∂I/∂y, Blue = magnitude
        color = vec3(abs(dI_dx) * 20.0, abs(dI_dy) * 20.0, spatialGradMag * 10.0);
    }
    else if(visualMode == 1) // ANGULAR GRADIENTS  
    {
        // Red = ∂I/∂θ, Green = ∂I/∂φ, Blue = magnitude
        color = vec3(abs(dI_dtheta) * 5.0, abs(dI_dphi) * 5.0, angularGradMag * 3.0);
    }
    else if(visualMode == 2) // COMBINED GRADIENT
    {
        // HSV mapping: Hue = direction, Saturation = magnitude
        float angle = atan(dI_dy, dI_dx) / (2.0 * 3.14159) + 0.5;
        color = vec3(angle, spatialGradMag * 10.0, intensity);
    }
    else if(visualMode == 3) // GRADIENT HEATMAP
    {
        // Heat map of total gradient magnitude
        float heat = clamp(totalGradMag * 2.0, 0.0, 1.0);
        if(heat < 0.33)
            color = mix(vec3(0,0,1), vec3(0,1,1), heat * 3.0); // Blue to Cyan
        else if(heat < 0.66)
            color = mix(vec3(0,1,1), vec3(1,1,0), (heat-0.33) * 3.0); // Cyan to Yellow
        else
            color = mix(vec3(1,1,0), vec3(1,0,0), (heat-0.66) * 3.0); // Yellow to Red
    }
    
    FragColor = vec4(color, 1.0);
}
void gradient()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    // Calculate the dot product (our main function)
    float ndoth = max(dot(norm, halfwayDir), 0.0);
    
    // Approximate spatial gradient magnitude
    float dx = dFdx(ndoth);
    float dy = dFdy(ndoth);
    float gradMag = sqrt(dx*dx + dy*dy);
    
    // Visualize gradient magnitude as color intensity
    vec3 gradientColor = vec3(gradMag * 10.0); // Amplify for visibility
    
    FragColor = vec4(gradientColor, 1.0);
}
void lights()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular - THE GRADIENT MAGIC HAPPENS HERE! ✨
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0;
    
    if(blinn)
    {
        // BLINN-PHONG: The smooth gradient king 👑
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    }
    else
    {
        // PHONG: The janky gradient peasant 💀
        vec3 reflectDir = reflect(-lightDir, norm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    }
    
    vec3 specular = spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}

/* class GradientVisualizer {
    int visualMode = 0;
    bool keyPressed = false;
    
public:
    void handleInput(GLFWwindow* window) {
        if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !keyPressed) {
            visualMode = 0; // Spatial gradients
            keyPressed = true;
            std::cout << "🔴 SPATIAL GRADIENTS MODE" << std::endl;
        }
        else if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !keyPressed) {
            visualMode = 1; // Angular gradients  
            keyPressed = true;
            std::cout << "🟡 ANGULAR GRADIENTS MODE" << std::endl;
        }
        else if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !keyPressed) {
            visualMode = 2; // Combined
            keyPressed = true;
            std::cout << "🟢 COMBINED GRADIENT MODE" << std::endl;
        }
        else if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && !keyPressed) {
            visualMode = 3; // Heatmap
            keyPressed = true;  
            std::cout << "🔥 HEATMAP MODE" << std::endl;
        }
        
        if(glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE && 
           glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE &&
           glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE &&
           glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) {
            keyPressed = false;
        }
    }
    
    void setUniforms(unsigned int shaderID) {
        glUniform1i(glGetUniformLocation(shaderID, "visualMode"), visualMode);
    }
};*/
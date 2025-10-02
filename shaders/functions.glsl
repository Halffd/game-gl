#version 330 core
out vec4 FragColor;

vec4 grayscale(vec4 color) {
    float average = (color.r + color.g + color.b) / 3.0;
    return vec4(average, average, average, color.a);
}
vec4 gradient(vec2 uv, vec4 color1, vec4 color2) {
    return mix(color1, color2, uv.x);
}
vec4 circularGradient(vec2 uv, vec4 color1, vec4 color2) {
    float dist = length(uv - vec2(0.5, 0.5)); // Calculate the distance from the center (0.5, 0.5)
    return mix(color1, color2, dist);
}
vec4 checkerboard(vec2 uv, float size) {
    vec2 grid = floor(uv * size);
    float checker = mod(grid.x + grid.y, 2.0);
    return vec4(checker, checker, checker, 1.0);
}
vec4 stairedGradient(vec2 uv, float size, vec4 color1, vec4 color2) {
    vec2 grid = floor(uv * size);
    float checker = grid.x / grid.y;
    return mix(color1, color2, checker);
}
// Vertical lines function
vec4 verticalLines(vec2 uv, float size, vec4 color1, vec4 color2) {
    vec2 grid = floor(uv * size);
    float checker = mod(grid.x, 2.0);
    return mix(color1, color2, checker);
}

// Horizontal lines function
vec4 horizontalLines(vec2 uv, float size, vec4 color1, vec4 color2) {
    vec2 grid = floor(uv * size);
    float checker = mod(grid.y, 2.0);
    return mix(color1, color2, checker);
}

// Example function to integrate - replace this with your actual function
float sampleFunction(vec2 p) {
    // Example: x² + y²
    return p.x * p.x + p.y * p.y;
}

// 2D Riemann sum approximation in shader
float riemann2D(vec2 startPos, vec2 endPos, int gridSizeX, int gridSizeY) {
    float totalSum = 0.0;
    
    float deltaX = (endPos.x - startPos.x) / float(gridSizeX);
    float deltaY = (endPos.y - startPos.y) / float(gridSizeY);
    
    // Ensure we don't have zero or negative grid sizes
    if (gridSizeX < 1) gridSizeX = 1;
    if (gridSizeY < 1) gridSizeY = 1;
    
    for(int i = 0; i < gridSizeX; i++) {
        for(int j = 0; j < gridSizeY; j++) {
            // Sample point at grid (i,j)
            vec2 samplePoint = startPos + vec2(
                (float(i) + 0.5) * deltaX,  // Center of rectangle
                (float(j) + 0.5) * deltaY
            );
            
            // Evaluate the function at the sample point
            float functionValue = sampleFunction(samplePoint);
            
            // Add weighted contribution (Riemann sum)
            totalSum += functionValue * deltaX * deltaY;
        }
    }
    
    return totalSum;
}

// Integrate Blinn-Phong lighting over a surface patch
float integrateLighting(vec3 surfacePos, vec3 normal, vec3 lightPos, 
                       float patchWidth, float patchHeight, int samples,
                       vec3 cameraPos, float shininess) {
    float totalLight = 0.0;
    
    // Ensure we have at least one sample
    samples = max(samples, 1);
    
    float dx = patchWidth / float(samples);
    float dy = patchHeight / float(samples);
    
    for(int i = 0; i < samples; i++) {
        for(int j = 0; j < samples; j++) {
            // Calculate sample position in the patch
            vec3 samplePos = surfacePos + vec3(
                (float(i) + 0.5) * dx - patchWidth * 0.5,
                (float(j) + 0.5) * dy - patchHeight * 0.5,
                0.0
            );
            
            // Calculate lighting at this sample point
            vec3 lightDir = normalize(lightPos - samplePos);
            vec3 viewDir = normalize(cameraPos - samplePos);
            vec3 halfwayDir = normalize(lightDir + viewDir);
            
            // Calculate specular component
            float specAngle = max(dot(normal, halfwayDir), 0.0);
            float intensity = pow(specAngle, shininess);
            
            // Add weighted contribution (Riemann sum)
            totalLight += intensity * dx * dy;
        }
    }
    
    return totalLight;
}
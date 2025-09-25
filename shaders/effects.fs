#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int u_effectType;      // Which effect to use
uniform float u_intensity;     // Effect intensity (0.0 - 1.0)
uniform vec2 u_screenSize;     // Screen dimensions for proper offset calculation

// Effect type constants (pass these from C++)
const int EFFECT_NONE = 0;
const int EFFECT_INVERT = 1;
const int EFFECT_GRAYSCALE = 2;
const int EFFECT_GRAYSCALE_WEIGHTED = 3;
const int EFFECT_SHARPEN = 4;
const int EFFECT_BLUR_GAUSSIAN = 5;
const int EFFECT_BLUR_BOX = 6;
const int EFFECT_EDGE_DETECTION = 7;
const int EFFECT_EMBOSS = 8;
const int EFFECT_EDGE_ENHANCE = 9;
const int EFFECT_SEPIA = 10;
const int EFFECT_VIGNETTE = 11;

// ===== UTILITY FUNCTIONS =====

vec3 sampleKernel3x3(float kernel[9]) {
    float offset = 1.0 / min(u_screenSize.x, u_screenSize.y);
    
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), vec2(0.0,    offset), vec2(offset,  offset),
        vec2(-offset,  0.0),    vec2(0.0,    0.0),    vec2(offset,  0.0),
        vec2(-offset, -offset), vec2(0.0,   -offset), vec2(offset, -offset)
    );
    
    vec3 color = vec3(0.0);
    for(int i = 0; i < 9; i++) {
        color += vec3(texture(screenTexture, TexCoords + offsets[i])) * kernel[i];
    }
    return color;
}

// ===== BASIC EFFECTS =====

vec3 effectNone() {
    return texture(screenTexture, TexCoords).rgb;
}

vec3 effectInvert() {
    return 1.0 - texture(screenTexture, TexCoords).rgb;
}

vec3 effectGrayscale() {
    vec3 color = texture(screenTexture, TexCoords).rgb;
    float gray = (color.r + color.g + color.b) / 3.0;
    return vec3(gray);
}

vec3 effectGrayscaleWeighted() {
    vec3 color = texture(screenTexture, TexCoords).rgb;
    float gray = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b; // Better weights
    return vec3(gray);
}

vec3 effectSepia() {
    vec3 color = texture(screenTexture, TexCoords).rgb;
    return vec3(
        dot(color, vec3(0.393, 0.769, 0.189)),
        dot(color, vec3(0.349, 0.686, 0.168)),
        dot(color, vec3(0.272, 0.534, 0.131))
    );
}

// ===== KERNEL EFFECTS =====

vec3 effectSharpen() {
    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    return sampleKernel3x3(kernel);
}

vec3 effectBlurGaussian() {
    float kernel[9] = float[](
        1.0/16, 2.0/16, 1.0/16,
        2.0/16, 4.0/16, 2.0/16,
        1.0/16, 2.0/16, 1.0/16
    );
    return sampleKernel3x3(kernel);
}

vec3 effectBlurBox() {
    float kernel[9] = float[](
        1.0/9, 1.0/9, 1.0/9,
        1.0/9, 1.0/9, 1.0/9,
        1.0/9, 1.0/9, 1.0/9
    );
    return sampleKernel3x3(kernel);
}

vec3 effectEdgeDetection() {
    float kernel[9] = float[](
        1,  1,  1,
        1, -8,  1,
        1,  1,  1
    );
    return sampleKernel3x3(kernel);
}

vec3 effectEmboss() {
    float kernel[9] = float[](
        -2, -1,  0,
        -1,  1,  1,
         0,  1,  2
    );
    return sampleKernel3x3(kernel);
}

vec3 effectEdgeEnhance() {
    float kernel[9] = float[](
         0, -1,  0,
        -1,  5, -1,
         0, -1,  0
    );
    return sampleKernel3x3(kernel);
}

// ===== ADVANCED EFFECTS =====

vec3 effectVignette() {
    vec3 color = texture(screenTexture, TexCoords).rgb;
    
    // Calculate distance from center
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(TexCoords, center);
    
    // Vignette strength (adjust these values)
    float innerRadius = 0.3;
    float outerRadius = 0.8;
    
    float vignette = 1.0 - smoothstep(innerRadius, outerRadius, dist);
    return color * vignette;
}

// ===== MAIN FUNCTION =====

void main() {
    vec3 originalColor = texture(screenTexture, TexCoords).rgb;
    vec3 effectColor;
    
    // Apply selected effect
    if (u_effectType == EFFECT_NONE) {
        effectColor = effectNone();
    }
    else if (u_effectType == EFFECT_INVERT) {
        effectColor = effectInvert();
    }
    else if (u_effectType == EFFECT_GRAYSCALE) {
        effectColor = effectGrayscale();
    }
    else if (u_effectType == EFFECT_GRAYSCALE_WEIGHTED) {
        effectColor = effectGrayscaleWeighted();
    }
    else if (u_effectType == EFFECT_SHARPEN) {
        effectColor = effectSharpen();
    }
    else if (u_effectType == EFFECT_BLUR_GAUSSIAN) {
        effectColor = effectBlurGaussian();
    }
    else if (u_effectType == EFFECT_BLUR_BOX) {
        effectColor = effectBlurBox();
    }
    else if (u_effectType == EFFECT_EDGE_DETECTION) {
        effectColor = effectEdgeDetection();
    }
    else if (u_effectType == EFFECT_EMBOSS) {
        effectColor = effectEmboss();
    }
    else if (u_effectType == EFFECT_EDGE_ENHANCE) {
        effectColor = effectEdgeEnhance();
    }
    else if (u_effectType == EFFECT_SEPIA) {
        effectColor = effectSepia();
    }
    else if (u_effectType == EFFECT_VIGNETTE) {
        effectColor = effectVignette();
    }
    else {
        effectColor = originalColor; // Fallback
    }
    
    // Mix original with effect based on intensity
    vec3 finalColor = mix(originalColor, effectColor, clamp(u_intensity, 0.0, 1.0));
    
    FragColor = vec4(finalColor, 1.0);
}
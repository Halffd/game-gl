#ifndef DYNAMIC_TRANSFORM_H
#define DYNAMIC_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <string>

namespace m3D {

// Enum for different transformation modes
enum class TransformMode {
    STANDARD,           // Standard transformation for most shapes
    CARTESIAN_PLANE,    // Special mode for CartesianPlane
    ORBIT,              // Orbiting motion
    WAVE,               // Wave-like motion
    SPIRAL              // Spiral motion
};

// Class to handle dynamic transformations based on time
class DynamicTransform {
public:
    DynamicTransform(
        const glm::vec3& basePosition = glm::vec3(0.0f),
        const glm::vec3& baseRotation = glm::vec3(0.0f),
        const glm::vec3& baseScale = glm::vec3(1.0f),
        const glm::vec3& baseColor = glm::vec3(1.0f),
        const glm::vec3& colorGradient = glm::vec3(0.2f, 0.3f, 0.4f),
        TransformMode mode = TransformMode::STANDARD
    ) : 
        basePosition(basePosition),
        baseRotation(baseRotation),
        baseScale(baseScale),
        baseColor(baseColor),
        colorGradient(colorGradient),
        diffuseBase(0.8f),
        ambientBase(0.2f),
        specularBase(0.5f),
        transformMode(mode)
    {}
    
    // Set the transform mode
    void setTransformMode(TransformMode mode) {
        transformMode = mode;
    }
    
    // Update transformations based on time
    void update(float time) {
        switch (transformMode) {
            case TransformMode::CARTESIAN_PLANE:
                updateCartesianPlane(time);
                break;
            case TransformMode::ORBIT:
                updateOrbit(time);
                break;
            case TransformMode::WAVE:
                updateWave(time);
                break;
            case TransformMode::SPIRAL:
                updateSpiral(time);
                break;
            case TransformMode::STANDARD:
            default:
                updateStandard(time);
                break;
        }
    }
    
    // Getters for current transformation values
    glm::vec3 getPosition() const { return currentPosition; }
    glm::vec3 getRotation() const { return currentRotation; }
    glm::vec3 getScale() const { return currentScale; }
    glm::vec3 getColor() const { return currentColor; }
    float getDiffuse() const { return currentDiffuse; }
    float getAmbient() const { return currentAmbient; }
    float getSpecular() const { return currentSpecular; }
    
private:
    // Standard update method (original implementation)
    void updateStandard(float time) {
        // Scale by cos(t)
        currentScale = baseScale * std::cos(time);
        
        // Rotate by sin(t)
        currentRotation = baseRotation + glm::vec3(
            std::sin(time),
            std::sin(time * 1.3f),
            std::sin(time * 0.7f)
        );
        
        // Translate by sec²(t)
        float secSquared = 1.0f / (std::cos(time) * std::cos(time));
        // Clamp to avoid extreme values
        secSquared = glm::clamp(secSquared, -10.0f, 10.0f);
        currentPosition = basePosition + glm::vec3(
            secSquared * 0.1f,
            secSquared * 0.05f,
            secSquared * 0.15f
        );
        
        // Change color gradient by arccot(t)
        float arccot = std::atan(1.0f / time);
        currentColor = baseColor + colorGradient * arccot;
        
        // Clamp color values to valid range
        currentColor = glm::clamp(currentColor, glm::vec3(0.0f), glm::vec3(1.0f));
        
        // Material properties
        // Diffuse varies by tan(t)
        float tanValue = std::tan(time);
        // Clamp to avoid extreme values
        tanValue = glm::clamp(tanValue, -5.0f, 5.0f);
        currentDiffuse = diffuseBase + tanValue * 0.1f;
        currentDiffuse = glm::clamp(currentDiffuse, 0.0f, 1.0f);
        
        // Ambient by cot(t)
        float cotValue = 1.0f / std::tan(time + 0.01f); // Add small offset to avoid division by zero
        // Clamp to avoid extreme values
        cotValue = glm::clamp(cotValue, -5.0f, 5.0f);
        currentAmbient = ambientBase + cotValue * 0.05f;
        currentAmbient = glm::clamp(currentAmbient, 0.0f, 1.0f);
        
        // Specular by arcsinh(t)
        float arcsinh = std::asinh(time);
        currentSpecular = specularBase + arcsinh * 0.1f;
        currentSpecular = glm::clamp(currentSpecular, 0.0f, 1.0f);
    }
    
    // Special update method for CartesianPlane
    void updateCartesianPlane(float time) {
        // Keep scale constant for CartesianPlane
        currentScale = baseScale;
        
        // Much slower rotation around Y axis with very slight wobble
        currentRotation = baseRotation + glm::vec3(
            std::sin(time * 0.05f) * 2.0f,  // Very small X rotation (tilt)
            time * 2.0f,                    // Much slower Y rotation (reduced from 10.0f to 2.0f)
            std::sin(time * 0.08f) * 2.0f   // Very small Z rotation (tilt)
        );
        
        // More gentle floating motion
        currentPosition = basePosition + glm::vec3(
            std::sin(time * 0.1f) * 1.0f,   // Reduced amplitude and frequency
            std::cos(time * 0.08f) * 0.5f,  // Reduced amplitude and frequency
            std::sin(time * 0.12f) * 1.0f   // Reduced amplitude and frequency
        );
        
        // Keep color constant for CartesianPlane
        currentColor = baseColor;
        
        // Material properties remain constant
        currentDiffuse = diffuseBase;
        currentAmbient = ambientBase;
        currentSpecular = specularBase;
    }
    
    // Orbiting motion
    void updateOrbit(float time) {
        // Scale pulsates very slightly
        currentScale = baseScale * (1.0f + 0.05f * std::sin(time * 0.5f));
        
        // Much slower rotation - reduced from 30.0f to 1.0f
        currentRotation = baseRotation + glm::vec3(
            0.0f, 
            time * 1.0f, // Very slow Y rotation
            0.0f
        );
        
        // Orbit around a center point with very gentle movement
        float radius = 2.0f;
        currentPosition = basePosition + glm::vec3(
            radius * 0.1f * std::cos(time * 0.2f), // Very small X movement
            0.2f * std::sin(time * 0.1f),          // Very gentle Y bobbing
            radius * 0.1f * std::sin(time * 0.2f)  // Very small Z movement
        );
        
        // Keep color mostly constant with very subtle changes
        currentColor = baseColor * (0.95f + 0.05f * std::sin(time * 0.1f));
        
        // Material properties remain constant
        currentDiffuse = diffuseBase;
        currentAmbient = ambientBase;
        currentSpecular = specularBase;
    }
    
    // Wave-like motion
    void updateWave(float time) {
        // Scale varies in a wave pattern
        currentScale = baseScale * (1.0f + 0.3f * std::sin(time));
        
        // Rotation follows the wave
        currentRotation = baseRotation + glm::vec3(
            30.0f * std::sin(time * 0.5f),
            45.0f * std::sin(time * 0.3f),
            15.0f * std::sin(time * 0.7f)
        );
        
        // Position follows a wave pattern
        currentPosition = basePosition + glm::vec3(
            std::sin(time) * 3.0f,
            std::cos(time * 1.5f) * 2.0f,
            std::sin(time * 0.7f) * 3.0f
        );
        
        // Color varies with the wave
        float wave = 0.5f + 0.5f * std::sin(time);
        currentColor = baseColor * wave + colorGradient * (1.0f - wave);
        
        // Material properties
        currentDiffuse = diffuseBase;
        currentAmbient = ambientBase;
        currentSpecular = specularBase;
    }
    
    // Spiral motion
    void updateSpiral(float time) {
        // Scale increases and decreases
        currentScale = baseScale * (1.0f + 0.5f * std::sin(time * 0.5f));
        
        // Rotation spirals
        currentRotation = baseRotation + glm::vec3(
            time * 20.0f,
            time * 15.0f,
            time * 10.0f
        );
        
        // Position follows a spiral
        float spiralRadius = 3.0f * (1.0f + 0.5f * std::sin(time * 0.2f));
        float spiralHeight = 2.0f * std::sin(time * 0.3f);
        currentPosition = basePosition + glm::vec3(
            spiralRadius * std::cos(time),
            spiralHeight,
            spiralRadius * std::sin(time)
        );
        
        // Color spirals through hues
        float hue = std::fmod(time * 0.1f, 1.0f);
        float h = hue * 6.0f;
        int i = static_cast<int>(h);
        float f = h - i;
        float p = 0.0f;
        float q = 1.0f - f;
        float t = f;
        
        switch (i % 6) {
            case 0: currentColor = glm::vec3(1.0f, t, p); break;
            case 1: currentColor = glm::vec3(q, 1.0f, p); break;
            case 2: currentColor = glm::vec3(p, 1.0f, t); break;
            case 3: currentColor = glm::vec3(p, q, 1.0f); break;
            case 4: currentColor = glm::vec3(t, p, 1.0f); break;
            case 5: currentColor = glm::vec3(1.0f, p, q); break;
        }
        
        // Material properties
        currentDiffuse = diffuseBase;
        currentAmbient = ambientBase;
        currentSpecular = specularBase;
    }
    
    // Base values
    glm::vec3 basePosition;
    glm::vec3 baseRotation;
    glm::vec3 baseScale;
    glm::vec3 baseColor;
    glm::vec3 colorGradient;
    float diffuseBase;
    float ambientBase;
    float specularBase;
    TransformMode transformMode;
    
    // Current values
    glm::vec3 currentPosition;
    glm::vec3 currentRotation;
    glm::vec3 currentScale;
    glm::vec3 currentColor;
    float currentDiffuse;
    float currentAmbient;
    float currentSpecular;
};

} // namespace m3D

#endif // DYNAMIC_TRANSFORM_H 
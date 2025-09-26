// RearViewMirror.h
#pragma once
#include "render/Framebuffer.hpp"
#include "render/Shader.h"
#include "render/primitives/2d/2D.hpp"
#include "asset/ResourceManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>
#include <memory>
class Mirror {
private:
    std::unique_ptr<Framebuffer> m_mirrorFramebuffer;
    std::shared_ptr<VO::Quad> m_quad;
    Shader m_mirrorShader;
    
    // Mirror properties
    int m_mirrorWidth = 256;   // Lower res for performance
    int m_mirrorHeight = 128;  // 2:1 aspect ratio like real mirrors
    glm::vec2 m_screenPos = {0.7f, 0.85f}; // Top-right corner
    glm::vec2 m_screenSize = {0.25f, 0.12f}; // 25% width, 12% height
    
public:
    Mirror() = default;
    ~Mirror() { cleanup(); }
    
    bool initialize() {
        // Create mirror framebuffer
        m_mirrorFramebuffer = std::make_unique<Framebuffer>();
        if (!m_mirrorFramebuffer->create(m_mirrorWidth, m_mirrorHeight)) {
            return false;
        }
        
        // Create quad for mirror display
        m_quad = std::make_shared<VO::Quad>();
        
        // Load mirror shader
        m_mirrorShader = loadMirrorShader();
        
        return true;
    }
    
    // Call this to render the mirror view
    void renderMirrorView(const glm::mat4& originalView, const glm::mat4& projection, 
                         std::function<void(const glm::mat4&, const glm::mat4&)> renderScene) {
        
        // Create rear-view camera matrix (180° rotation)
        glm::mat4 mirrorView = createRearViewMatrix(originalView);
        
        // Render to mirror framebuffer
        m_mirrorFramebuffer->bind();
        m_mirrorFramebuffer->clear(0.1f, 0.1f, 0.3f); // Sky-ish color
        
        // Render scene from rear view
        renderScene(mirrorView, projection);
        
        m_mirrorFramebuffer->unbind();
    }
    
    // Call this to draw the mirror on screen
    void drawMirror(int screenWidth, int screenHeight) {
        // Disable depth testing for overlay
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        m_mirrorShader.Use();
        
        // Set mirror position and size
        glm::vec2 pos = {
            (m_screenPos.x - m_screenSize.x * 0.5f) * 2.0f - 1.0f,
            (m_screenPos.y - m_screenSize.y * 0.5f) * 2.0f - 1.0f
        };
        glm::vec2 size = m_screenSize * 2.0f;
        
        m_mirrorShader.SetVector2f("u_position", pos);
        m_mirrorShader.SetVector2f("u_size", size);
        
        // Bind mirror texture
        m_mirrorFramebuffer->bindColorTexture(0);
        
        // Draw mirror quad
        m_quad->draw();
        
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }
    
private:
    glm::mat4 createRearViewMatrix(const glm::mat4& originalView) {
        // Extract camera position and direction from view matrix
        glm::mat4 invView = glm::inverse(originalView);
        glm::vec3 cameraPos = glm::vec3(invView[3]);
        glm::vec3 cameraFront = -glm::vec3(invView[2]); // Forward direction
        glm::vec3 cameraUp = glm::vec3(invView[1]);
        
        // Create rear view (180° rotation around up axis)
        glm::vec3 rearDirection = -cameraFront; // Look backwards
        
        return glm::lookAt(cameraPos, cameraPos + rearDirection, cameraUp);
    }
    
    void setupMirrorQuad() {
        m_quad->setup();
    }
    
    Shader loadMirrorShader() {
         ResourceManager::LoadShader("mirror.vs", "mirror.fs", "mirror");
         return ResourceManager::GetShader("mirror");
    }
    
    void cleanup() {
        m_mirrorFramebuffer.reset();
        m_quad.reset();
    }
};
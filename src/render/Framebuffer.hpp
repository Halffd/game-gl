#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <glad/glad.h>
#include <iostream>
#include <vector>
#include "util/Util.h"

class Framebuffer {
private:
    GLuint m_fbo = 0;
    GLuint m_colorTexture = 0;
    GLuint m_depthStencilRBO = 0;
    int m_width, m_height;
    
public:
    Framebuffer() = default;
    Framebuffer(const Framebuffer& other) = delete;
    Framebuffer(Framebuffer&& other) noexcept { *this = std::move(other); }
    
    ~Framebuffer() {
        cleanup();
    }
    
    Framebuffer& operator=(const Framebuffer& other) = delete;
    Framebuffer& operator=(Framebuffer&& other) noexcept {
        if (this != &other) {
            cleanup();
            
            // Manual moves - no std::exchange needed
            m_fbo = other.m_fbo;
            other.m_fbo = 0;
            
            m_colorTexture = other.m_colorTexture;
            other.m_colorTexture = 0;
            
            m_depthStencilRBO = other.m_depthStencilRBO;
            other.m_depthStencilRBO = 0;
            
            m_width = other.m_width;
            m_height = other.m_height;
        }
        return *this;
    }
    
    /**
     * @brief Create framebuffer with specified dimensions
     */
    bool create(int width, int height) {
        cleanup(); // Clean up any existing resources
        
        m_width = width;
        m_height = height;
        
        // Generate framebuffer
        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glCheckError(__FILE__, __LINE__);
        
        // Create color texture attachment
        if (!createColorTexture()) {
            cleanup();
            return false;
        }
        
        // Create depth/stencil renderbuffer attachment
        if (!createDepthStencilRBO()) {
            cleanup();
            return false;
        }
        
        // Check framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR::FRAMEBUFFER: Framebuffer is not complete!" << std::endl;
            cleanup();
            return false;
        }
        
        // Unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return true;
    }
    
    /**
     * @brief Bind this framebuffer for rendering
     */
    void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glViewport(0, 0, m_width, m_height);
        glCheckError(__FILE__, __LINE__);
    }
    
    /**
     * @brief Unbind framebuffer (bind default framebuffer)
     */
    void unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glCheckError(__FILE__, __LINE__);
    }
    
    /**
     * @brief Bind the color texture for sampling
     */
    void bindColorTexture(int textureUnit = 0) const {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, m_colorTexture);
        glCheckError(__FILE__, __LINE__);
    }
    
    /**
     * @brief Clear the framebuffer
     */
    void clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) const {
        bind();
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glCheckError(__FILE__, __LINE__);
    }
    
    /**
     * @brief Resize the framebuffer
     */
    bool resize(int width, int height) {
        if (width == m_width && height == m_height) return true;
        return create(width, height);
    }
    
    // Getters
    GLuint getFBO() const { return m_fbo; }
    GLuint getColorTexture() const { return m_colorTexture; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
private:
    void cleanup() {
        if (m_colorTexture) {
            glDeleteTextures(1, &m_colorTexture);
            m_colorTexture = 0;
        }
        if (m_depthStencilRBO) {
            glDeleteRenderbuffers(1, &m_depthStencilRBO);
            m_depthStencilRBO = 0;
        }
        if (m_fbo) {
            glDeleteFramebuffers(1, &m_fbo);
            m_fbo = 0;
        }
        glCheckError(__FILE__, __LINE__);
    }
    
    bool createColorTexture() {
        glGenTextures(1, &m_colorTexture);
        glBindTexture(GL_TEXTURE_2D, m_colorTexture);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Attach to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);
        
        glCheckError(__FILE__, __LINE__);
        return m_colorTexture != 0;
    }
    
    bool createDepthStencilRBO() {
        glGenRenderbuffers(1, &m_depthStencilRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        
        // Attach to framebuffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilRBO);
        
        glCheckError(__FILE__, __LINE__);
        return m_depthStencilRBO != 0;
    }
};

// Multi-target framebuffer for advanced effects
class MultitargetFramebuffer {
private:
    GLuint m_fbo = 0;
    std::vector<GLuint> m_colorTextures;
    GLuint m_depthStencilRBO = 0;
    int m_width, m_height;
    
public:
    MultitargetFramebuffer() = default;
    ~MultitargetFramebuffer() { cleanup(); }
    
    // Non-copyable, movable
    MultitargetFramebuffer(const MultitargetFramebuffer&) = delete;
    MultitargetFramebuffer& operator=(const MultitargetFramebuffer&) = delete;
    MultitargetFramebuffer(MultitargetFramebuffer&&) = default;
    MultitargetFramebuffer& operator=(MultitargetFramebuffer&&) = default;
    
    bool create(int width, int height, int colorAttachments = 1) {
        cleanup();
        
        m_width = width;
        m_height = height;
        m_colorTextures.resize(colorAttachments);
        
        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        
        // Create color attachments
        std::vector<GLenum> drawBuffers;
        for (int i = 0; i < colorAttachments; ++i) {
            glGenTextures(1, &m_colorTextures[i]);
            glBindTexture(GL_TEXTURE_2D, m_colorTextures[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorTextures[i], 0);
            drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
        
        glDrawBuffers(drawBuffers.size(), drawBuffers.data());
        
        // Depth/stencil
        glGenRenderbuffers(1, &m_depthStencilRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilRBO);
        
        bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        return complete;
    }
    
    void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glViewport(0, 0, m_width, m_height);
    }
    
    void bindColorTexture(int attachment, int textureUnit) const {
        if (attachment >= 0 && static_cast<size_t>(attachment) < m_colorTextures.size()) {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, m_colorTextures[attachment]);
        }
    }
    
private:
    void cleanup() {
        for (auto tex : m_colorTextures) {
            if (tex) glDeleteTextures(1, &tex);
        }
        m_colorTextures.clear();
        
        if (m_depthStencilRBO) glDeleteRenderbuffers(1, &m_depthStencilRBO);
        if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
        
        m_fbo = m_depthStencilRBO = 0;
    }
};

#endif
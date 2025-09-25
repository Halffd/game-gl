#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <algorithm>
#include <cstring>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <memory>
#include "util/Util.h"
#include "stb_image_write.h"
#include <ctime>
#include <chrono>
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
     * @brief Save framebuffer contents to image file
     * @param filename Output file path (supports .png, .jpg, .bmp, .tga)
     * @param flipVertically OpenGL textures are upside down by default
     * @return true if successful
     */
     bool screenshot(const std::string& filename, bool flipVertically = true) const {
        if (m_fbo == 0) {
            std::cerr << "ERROR::FRAMEBUFFER: Cannot screenshot - framebuffer not initialized!" << std::endl;
            return false;
        }
        
        // Bind this framebuffer to read from it
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
        
        // Allocate buffer for pixel data
        std::unique_ptr<unsigned char[]> pixels(new unsigned char[m_width * m_height * 3]);
        
        // Read pixels from framebuffer
        glReadPixels(0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, pixels.get());
        glCheckError(__FILE__, __LINE__);
        
        // Flip vertically if requested (OpenGL origin is bottom-left)
        if (flipVertically) {
            flipImageVertically(pixels.get(), m_width, m_height, 3);
        }
        
        // Determine file format and save
        bool success = false;
        std::string ext = getFileExtension(filename);
        
        if (ext == "png") {
            success = stbi_write_png(filename.c_str(), m_width, m_height, 3, pixels.get(), m_width * 3);
        } else if (ext == "jpg" || ext == "jpeg") {
            success = stbi_write_jpg(filename.c_str(), m_width, m_height, 3, pixels.get(), 90); // 90% quality
        } else if (ext == "bmp") {
            success = stbi_write_bmp(filename.c_str(), m_width, m_height, 3, pixels.get());
        } else if (ext == "tga") {
            success = stbi_write_tga(filename.c_str(), m_width, m_height, 3, pixels.get());
        } else {
            std::cerr << "ERROR::FRAMEBUFFER: Unsupported file format: " << ext << std::endl;
            success = false;
        }
        
        // Restore default framebuffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        
        if (success) {
            std::cout << "Screenshot saved: " << filename << " (" << m_width << "x" << m_height << ")" << std::endl;
        } else {
            std::cerr << "ERROR::FRAMEBUFFER: Failed to save screenshot: " << filename << std::endl;
        }
        
        return success;
    }
    
    /**
     * @brief Get raw pixel data from framebuffer
     * @param outPixels Output buffer (must be allocated: width * height * 3 bytes)
     * @param format GL_RGB, GL_RGBA, etc.
     * @param type GL_UNSIGNED_BYTE, GL_FLOAT, etc.
     */
    void getPixelData(void* outPixels, GLenum format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE) const {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
        glReadPixels(0, 0, m_width, m_height, format, type, outPixels);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
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
    
    
    // Helper functions
    static void flipImageVertically(unsigned char* data, int width, int height, int channels) {
        int rowSize = width * channels;
        std::unique_ptr<unsigned char[]> temp(new unsigned char[rowSize]);
        
        for (int y = 0; y < height / 2; ++y) {
            unsigned char* row1 = data + y * rowSize;
            unsigned char* row2 = data + (height - 1 - y) * rowSize;

            // Swap rows
            std::memcpy(temp.get(), row1, rowSize);
            std::memcpy(row1, row2, rowSize);
            std::memcpy(row2, temp.get(), rowSize);
        }
    }
    
    static std::string getFileExtension(const std::string& filename) {
        size_t dotPos = filename.find_last_of('.');
        if (dotPos == std::string::npos) return "";
        
        std::string ext = filename.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }
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
    

    /**
     * @brief Screenshot specific color attachment
     */
     bool screenshotAttachment(int attachment, const std::string& filename, bool flipVertically = true) const {
        if (attachment < 0 || static_cast<size_t>(attachment) >= m_colorTextures.size()) {
            std::cerr << "ERROR::FRAMEBUFFER: Invalid attachment index: " << attachment << std::endl;
            return false;
        }
        
        // Set read buffer to specific attachment
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
        
        // Allocate buffer for pixel data
        std::unique_ptr<unsigned char[]> pixels(new unsigned char[m_width * m_height * 4]);
        
        // Read pixels from framebuffer
        glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());
        glCheckError(__FILE__, __LINE__);
        
        // Flip vertically if requested (OpenGL origin is bottom-left)
        if (flipVertically) {
            Framebuffer::flipImageVertically(pixels.get(), m_width, m_height, 4);
        }
        
        // Determine file format and save
        bool success = false;
        std::string ext = Framebuffer::getFileExtension(filename);
        
        if (ext == "png") {
            success = stbi_write_png(filename.c_str(), m_width, m_height, 4, pixels.get(), m_width * 4);
        } else if (ext == "jpg" || ext == "jpeg") {
            success = stbi_write_jpg(filename.c_str(), m_width, m_height, 4, pixels.get(), 90); // 90% quality
        } else if (ext == "bmp") {
            success = stbi_write_bmp(filename.c_str(), m_width, m_height, 4, pixels.get());
        } else if (ext == "tga") {
            success = stbi_write_tga(filename.c_str(), m_width, m_height, 4, pixels.get());
        } else {
            std::cerr << "ERROR::FRAMEBUFFER: Unsupported file format: " << ext << std::endl;
            success = false;
        }
        
        // Restore default framebuffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        
        if (success) {
            std::cout << "Screenshot saved: " << filename << " (" << m_width << "x" << m_height << ")" << std::endl;
        } else {
            std::cerr << "ERROR::FRAMEBUFFER: Failed to save screenshot: " << filename << std::endl;
        }
        
        return success;
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
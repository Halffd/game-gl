// RearViewMirror.h
#pragma once
#include "render/Framebuffer.hpp"
#include "render/Shader.h"
#include "render/primitives/2d/2D.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include <memory>
class Mirror {
private:
    std::unique_ptr<Framebuffer> m_mirrorFramebuffer;
    std::shared_ptr<VO::Quad> m_quad;
    Shader m_mirrorShader;
    
    // Mirror properties
    int m_mirrorWidth = 1024;   // Higher resolution for better quality
    int m_mirrorHeight = 768;   // 4:3 aspect ratio
    glm::vec2 m_screenPos = {0.7f, 0.7f}; // Position on screen (0-1 range)
    glm::vec2 m_screenSize = {0.3f, 0.3f}; // Size of the mirror on screen (0-1 range)
    
public:
    Mirror() = default;
    ~Mirror() { cleanup(); }
    
    bool initialize() {
        // Create mirror framebuffer
        m_mirrorFramebuffer = std::make_unique<Framebuffer>();
        if (!m_mirrorFramebuffer->create(m_mirrorWidth, m_mirrorHeight)) {
            std::cerr << "Failed to create mirror framebuffer!" << std::endl;
            return false;
        }
        
        // Create quad for mirror display
        m_quad = std::make_shared<VO::Quad>();
        setupMirrorQuad();
        
        // Load mirror shader
        m_mirrorShader = loadMirrorShader();
        if (m_mirrorShader.ID == 0) {
            std::cerr << "Failed to load mirror shader!" << std::endl;
            return false;
        }
        
        std::cout << "Mirror initialized successfully" << std::endl;
        return true;
    }
    
    // Call this to render the mirror view
    void renderMirrorView(const glm::mat4& originalView, const glm::mat4& projection, 
                         std::function<void(const glm::mat4&, const glm::mat4&)> renderScene) {
        if (!m_mirrorFramebuffer) {
            std::cerr << "Mirror framebuffer not initialized!" << std::endl;
            return;
        }
        
        // Save current viewport
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        
        // Set viewport to mirror framebuffer size
        glViewport(0, 0, m_mirrorWidth, m_mirrorHeight);
        
        // Bind framebuffer and clear it
        m_mirrorFramebuffer->bind();
        glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render scene from mirror's perspective
        glm::mat4 mirrorView = createRearViewMatrix(originalView);
        renderScene(mirrorView, projection);
        
        // Restore framebuffer and viewport
        m_mirrorFramebuffer->unbind();
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    }
    
    // Call this to draw the mirror on screen
    void drawMirror(int screenWidth, int screenHeight) {
        if (!m_quad || m_mirrorShader.ID == 0) {
            std::cerr << "Mirror quad or shader not initialized!" << std::endl;
            return;
        }
        
        // Save current state
        GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
        GLboolean blend = glIsEnabled(GL_BLEND);
        
        // Set up for 2D rendering
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Use mirror shader
        m_mirrorShader.Use();
        
        // Calculate position and size in NDC
        glm::vec2 pos = {
            (m_screenPos.x - m_screenSize.x * 0.5f) * 2.0f - 1.0f,
            1.0f - (m_screenPos.y + m_screenSize.y * 0.5f) * 2.0f  // Flip Y for OpenGL coordinates
        };
        glm::vec2 size = m_screenSize * 2.0f;
        
        // Set shader uniforms
        m_mirrorShader.SetVector2f("u_position", pos);
        m_mirrorShader.SetVector2f("u_size", size);
        
        // Bind mirror texture to texture unit 0
        glActiveTexture(GL_TEXTURE0);
        m_mirrorFramebuffer->bindColorTexture(0);
        m_mirrorShader.SetInteger("mirrorTexture", 0);
        
        // Draw the mirror quad
        m_quad->draw();
        
        // Restore state
        if (depthTest) glEnable(GL_DEPTH_TEST);
        if (!blend) glDisable(GL_BLEND);
    }
    
private:
    glm::mat4 createRearViewMatrix(const glm::mat4& originalView) {
        // Extract camera position and direction from view matrix
        glm::mat4 invView = glm::inverse(originalView);
        glm::vec3 cameraPos = glm::vec3(invView[3]);
        glm::vec3 cameraFront = -glm::vec3(invView[2]); // Forward direction
        glm::vec3 cameraUp = glm::vec3(invView[1]);
        
        // For a rear-view mirror, we want to reflect the view across the mirror plane
        // which is typically the XZ plane (assuming Y is up)
        glm::vec3 mirrorNormal = glm::vec3(0.0f, 0.0f, -1.0f); // Mirror facing -Z
        float mirrorDistance = 0.0f; // Distance from camera to mirror plane along normal
        
        // Calculate reflection matrix
        glm::mat4 reflectionMatrix = glm::mat4(1.0f);
        reflectionMatrix[2][2] = -1.0f; // Reflect Z-axis
        
        // Calculate mirror position and orientation
        glm::vec3 mirrorPos = cameraPos - 2.0f * glm::dot(cameraPos + mirrorNormal * mirrorDistance, mirrorNormal) * mirrorNormal;
        glm::vec3 mirrorFront = reflect(cameraFront, mirrorNormal);
        glm::vec3 mirrorUp = reflect(cameraUp, mirrorNormal);
        
        return glm::lookAt(mirrorPos, mirrorPos + mirrorFront, mirrorUp) * reflectionMatrix;
    }
    
    void setupMirrorQuad() {
        m_quad->setup();
    }
    
    Shader loadMirrorShader() {
        // Create shader from source files
        Shader shader;
        
        // Load vertex shader
        std::string vertexCode;
        std::ifstream vShaderFile;
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            vShaderFile.open("shaders/mirror.vs");
            std::stringstream vShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            vShaderFile.close();
            vertexCode = vShaderStream.str();
        } catch (std::ifstream::failure& e) {
            std::cerr << "ERROR::SHADER::VERTEX::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
            return Shader();
        }
        
        // Load fragment shader
        std::string fragmentCode;
        std::ifstream fShaderFile;
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            fShaderFile.open("shaders/mirror.fs");
            std::stringstream fShaderStream;
            fShaderStream << fShaderFile.rdbuf();
            fShaderFile.close();
            fragmentCode = fShaderStream.str();
        } catch (std::ifstream::failure& e) {
            std::cerr << "ERROR::SHADER::FRAGMENT::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
            return Shader();
        }
        
        // Compile shaders
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        
        // Compile vertex shader
        unsigned int vertex, fragment;
        int success;
        char infoLog[512];
        
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            return Shader();
        }
        
        // Compile fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            glDeleteShader(vertex);
            return Shader();
        }
        
        // Link shaders
        unsigned int ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return Shader();
        }
        
        // Clean up shaders
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        
        // Create and return shader
        shader.ID = ID;
        shader.Use();
        shader.SetInteger("mirrorTexture", 0);
        
        return shader;
    }
    
    void cleanup() {
        m_mirrorFramebuffer.reset();
        m_quad.reset();
    }
};
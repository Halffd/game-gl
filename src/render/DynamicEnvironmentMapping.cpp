#include "DynamicEnvironmentMapping.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

DynamicEnvironmentMapping::DynamicEnvironmentMapping() 
    : captureFBO(0), captureRBO(0), envCubemap(0), shaderProgram(0) {
    // Initialize the view matrices for the 6 faces of the cubemap
    captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
    captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
    captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    
    // Create the projection matrix for the cubemap capture
    captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 25.0f);
}

DynamicEnvironmentMapping::~DynamicEnvironmentMapping() {
    cleanup();
}

void DynamicEnvironmentMapping::initialize() {
    // Generate the framebuffer
    glGenFramebuffers(1, &captureFBO);
    
    // Generate a texture for the cubemap
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, 
                     GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Generate a depth buffer for the cubemap
    glGenRenderbuffers(1, &captureRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    // Attach the depth buffer to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                              GL_RENDERBUFFER, captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DynamicEnvironmentMapping::cleanup() {
    if (captureFBO != 0) {
        glDeleteFramebuffers(1, &captureFBO);
        captureFBO = 0;
    }
    
    if (envCubemap != 0) {
        glDeleteTextures(1, &envCubemap);
        envCubemap = 0;
    }
    
    if (captureRBO != 0) {
        glDeleteRenderbuffers(1, &captureRBO);
        captureRBO = 0;
    }
    
    // Clean up all probes
    for (auto& probe : probes) {
        if (probe.framebuffer != 0) {
            glDeleteFramebuffers(1, &probe.framebuffer);
        }
        if (probe.textureID != 0) {
            glDeleteTextures(1, &probe.textureID);
        }
        if (probe.depthMap != 0) {
            glDeleteRenderbuffers(1, &probe.depthMap);
        }
    }
    probes.clear();
}

void DynamicEnvironmentMapping::addReflectionProbe(const glm::vec3& position) {
    ReflectionProbe probe(position);
    
    // Generate framebuffer for this probe
    glGenFramebuffers(1, &probe.framebuffer);
    
    // Generate texture for this probe's cubemap
    glGenTextures(1, &probe.textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, probe.textureID);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, 
                     GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Generate depth buffer for this probe
    glGenRenderbuffers(1, &probe.depthMap);
    glBindRenderbuffer(GL_RENDERBUFFER, probe.depthMap);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    // Attach depth buffer to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, probe.framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                              GL_RENDERBUFFER, probe.depthMap);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    probes.push_back(probe);
}

void DynamicEnvironmentMapping::updateProbes() {
    // In a real implementation, this would determine which probes need updating
    // based on scene changes or time intervals
    for (auto& probe : probes) {
        probe.needsUpdate = true; // For now, mark all for update
    }
}

void DynamicEnvironmentMapping::renderProbe(int probeIndex, 
                                          void (*renderSceneCallback)(const glm::mat4&, const glm::mat4&)) {
    if (probeIndex < 0 || probeIndex >= probes.size()) {
        std::cerr << "Invalid probe index: " << probeIndex << std::endl;
        return;
    }
    
    ReflectionProbe& probe = probes[probeIndex];
    
    // Bind the framebuffer for this probe
    glBindFramebuffer(GL_FRAMEBUFFER, probe.framebuffer);
    
    // Set the viewport to match the cubemap texture size
    glViewport(0, 0, 512, 512);
    
    // Clear the framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use the shader program
    glUseProgram(shaderProgram);
    
    // Render to each face of the cubemap
    for (unsigned int i = 0; i < 6; ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, probe.textureID, 0);
        
        // Calculate the view matrix for this face
        glm::mat4 view = captureViews[i];
        // Adjust the view matrix to be centered on the probe's position
        view = glm::translate(view, -probe.position);
        
        // Call the scene rendering callback with the appropriate projection and view matrices
        if (renderSceneCallback) {
            renderSceneCallback(captureProjection, view);
        }
    }
    
    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Mark as updated
    probe.needsUpdate = false;
}

unsigned int DynamicEnvironmentMapping::getProbeCubemap(int probeIndex) const {
    if (probeIndex < 0 || probeIndex >= probes.size()) {
        return 0;
    }
    return probes[probeIndex].textureID;
}

int DynamicEnvironmentMapping::getClosestProbe(const glm::vec3& position) const {
    if (probes.empty()) {
        return -1;
    }
    
    float minDistance = std::numeric_limits<float>::max();
    int closestIndex = 0;
    
    for (size_t i = 0; i < probes.size(); ++i) {
        float distance = glm::distance(position, probes[i].position);
        if (distance < minDistance) {
            minDistance = distance;
            closestIndex = static_cast<int>(i);
        }
    }
    
    return closestIndex;
}

void DynamicEnvironmentMapping::updateProbePosition(int index, const glm::vec3& newPosition) {
    if (index >= 0 && index < probes.size()) {
        probes[index].position = newPosition;
        probes[index].needsUpdate = true;
    }
}

void DynamicEnvironmentMapping::markForUpdate(int index) {
    if (index >= 0 && index < probes.size()) {
        probes[index].needsUpdate = true;
    }
}

void DynamicEnvironmentMapping::renderAllProbes(void (*renderSceneCallback)(const glm::mat4&, const glm::mat4&)) {
    for (size_t i = 0; i < probes.size(); ++i) {
        if (probes[i].needsUpdate) {
            renderProbe(static_cast<int>(i), renderSceneCallback);
        }
    }
}
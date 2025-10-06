#include "renderer/SkyBox.h"
#include "core/Camera.h"
#include <glad/glad.h>
#include <iostream>

namespace FlightSim {

SkyBox::SkyBox()
    : m_topColor(0.5f, 0.7f, 1.0f)
    , m_bottomColor(0.8f, 0.9f, 1.0f)
    , m_sunPosition(0.3f, 0.7f, 0.2f)
    , m_timeOfDay(0.5f)
    , m_sunIntensity(1.0f)
    , m_sunColor(1.0f, 0.9f, 0.7f)
    , m_horizonColor(0.9f, 0.8f, 0.7f) {
}

SkyBox::~SkyBox() {
    Shutdown();
}

bool SkyBox::Initialize() {
    SetupShaders();
    CreateSkyMesh();
    UpdateSkyColors();
    return true;
}

void SkyBox::Shutdown() {
    m_skyMesh.reset();
    m_skyShader.reset();
}

void SkyBox::Render(const Camera& camera) {
    if (!m_skyShader || !m_skyMesh) return;
    
    // Disable depth writing for skybox
    glDepthMask(GL_FALSE);
    
    m_skyShader->Use();
    
    // Remove translation from view matrix (keep only rotation)
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    glm::mat4 projection = camera.GetProjectionMatrix();
    
    m_skyShader->SetMat4("view", view);
    m_skyShader->SetMat4("projection", projection);
    
    // Set sky colors
    m_skyShader->SetVec3("topColor", m_topColor);
    m_skyShader->SetVec3("bottomColor", m_bottomColor);
    m_skyShader->SetVec3("sunPosition", m_sunPosition);
    m_skyShader->SetVec3("sunColor", m_sunColor);
    m_skyShader->SetFloat("sunIntensity", m_sunIntensity);
    
    // Render skybox
    m_skyMesh->Render();
    
    m_skyShader->Unbind();
    
    // Re-enable depth writing
    glDepthMask(GL_TRUE);
}

void SkyBox::SetSkyColor(const glm::vec3& topColor, const glm::vec3& bottomColor) {
    m_topColor = topColor;
    m_bottomColor = bottomColor;
}

void SkyBox::SetSunPosition(const glm::vec3& position) {
    m_sunPosition = glm::normalize(position);
}

void SkyBox::SetTimeOfDay(float time) {
    m_timeOfDay = time;
    UpdateSkyColors();
}

void SkyBox::CreateSkyMesh() {
    // Create a large cube for the skybox
    m_skyMesh = std::make_unique<Mesh>(Mesh::CreateCube());
    m_skyMesh->Upload();
}

void SkyBox::SetupShaders() {
    m_skyShader = std::make_unique<Shader>();
    
    std::string vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        
        out vec3 TexCoords;
        
        uniform mat4 projection;
        uniform mat4 view;
        
        void main() {
            TexCoords = aPos;
            vec4 pos = projection * view * vec4(aPos, 1.0);
            gl_Position = pos.xyww; // Set z to w so that z/w = 1.0 (max depth)
        }
    )";
    
    std::string fragmentSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 TexCoords;
        
        uniform vec3 topColor;
        uniform vec3 bottomColor;
        uniform vec3 sunPosition;
        uniform vec3 sunColor;
        uniform float sunIntensity;
        
        void main() {
            vec3 direction = normalize(TexCoords);
            
            // Enhanced vertical gradient with exponential falloff
            float t = (direction.y + 1.0) * 0.5;
            t = smoothstep(0.0, 1.0, t);
            
            // Create more realistic atmospheric gradient
            float atmosphereHeight = pow(t, 0.7);
            vec3 skyColor = mix(bottomColor, topColor, atmosphereHeight);
            
            // Enhanced sun with corona and glow
            float sunDot = dot(direction, sunPosition);
            
            // Sun disk
            float sunDisk = smoothstep(0.9998, 0.9999, sunDot);
            
            // Sun corona
            float sunCorona = pow(max(sunDot, 0.0), 128.0);
            
            // Sun glow (larger, softer)
            float sunGlow = pow(max(sunDot, 0.0), 16.0);
            
            vec3 sunEffect = (sunDisk * 5.0 + sunCorona * 2.0 + sunGlow * 0.5) * sunColor * sunIntensity;
            skyColor += sunEffect;
            
            // Enhanced horizon glow with color variation
            float horizonFactor = 1.0 - abs(direction.y);
            horizonFactor = pow(horizonFactor, 1.5);
            
            // Sunset/sunrise colors at horizon
            vec3 horizonColor = mix(vec3(1.0, 0.9, 0.8), vec3(1.0, 0.7, 0.5), 
                                    smoothstep(0.9, 1.0, abs(sunPosition.y)));
            skyColor = mix(skyColor, horizonColor, horizonFactor * 0.3);
            
            // Atmospheric scattering effect
            float scatter = pow(max(dot(direction, sunPosition), 0.0), 8.0);
            skyColor += scatter * sunColor * sunIntensity * 0.2;
            
            // Add subtle cloudiness with procedural noise
            float noise = fract(sin(dot(direction.xz, vec2(12.9898, 78.233))) * 43758.5453);
            float clouds = smoothstep(0.6, 0.8, noise) * smoothstep(-0.3, 0.3, direction.y);
            skyColor = mix(skyColor, vec3(1.0), clouds * 0.15);
            
            // Color grading for more vibrant sky
            skyColor = pow(skyColor, vec3(0.9));
            
            FragColor = vec4(skyColor, 1.0);
        }
    )";
    
    m_skyShader->LoadFromStrings(vertexSource, fragmentSource);
}

void SkyBox::UpdateSkyColors() {
    // Update colors based on time of day with smooth transitions
    float dayFactor = sin(m_timeOfDay * 3.14159f * 2.0f);
    
    // Calculate sunrise/sunset factor
    float sunriseFactor = 1.0f - abs(dayFactor);
    sunriseFactor = pow(sunriseFactor, 2.0f);
    
    if (dayFactor > 0.0f) {
        // Daytime colors - bright and vibrant
        glm::vec3 dayTop = glm::vec3(0.3f, 0.5f, 0.9f);
        glm::vec3 dayBottom = glm::vec3(0.6f, 0.8f, 1.0f);
        
        // Sunrise/sunset colors - warm and golden
        glm::vec3 sunriseTop = glm::vec3(0.8f, 0.5f, 0.7f);
        glm::vec3 sunriseBottom = glm::vec3(1.0f, 0.7f, 0.5f);
        
        // Blend between day and sunrise colors
        m_topColor = glm::mix(dayTop, sunriseTop, sunriseFactor);
        m_bottomColor = glm::mix(dayBottom, sunriseBottom, sunriseFactor);
        
        m_sunIntensity = 0.8f + 0.2f * dayFactor;
        m_sunColor = glm::mix(glm::vec3(1.0f, 0.95f, 0.8f), 
                              glm::vec3(1.0f, 0.6f, 0.3f), 
                              sunriseFactor);
    } else {
        // Twilight and night colors
        float nightFactor = abs(dayFactor);
        
        glm::vec3 twilightTop = glm::vec3(0.2f, 0.2f, 0.5f);
        glm::vec3 twilightBottom = glm::vec3(0.4f, 0.3f, 0.5f);
        
        glm::vec3 nightTop = glm::vec3(0.01f, 0.01f, 0.1f);
        glm::vec3 nightBottom = glm::vec3(0.05f, 0.05f, 0.15f);
        
        m_topColor = glm::mix(twilightTop, nightTop, nightFactor);
        m_bottomColor = glm::mix(twilightBottom, nightBottom, nightFactor);
        
        m_sunIntensity = 0.3f * (1.0f - nightFactor);
        m_sunColor = glm::vec3(1.0f, 0.5f, 0.3f);
    }
    
    // Update sun position based on time of day
    float sunAngle = m_timeOfDay * 3.14159f * 2.0f;
    m_sunPosition = glm::vec3(cos(sunAngle) * 0.5f, sin(sunAngle), -0.3f);
    m_sunPosition = glm::normalize(m_sunPosition);
}

} // namespace FlightSim 
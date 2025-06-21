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
            
            // Vertical gradient
            float t = (direction.y + 1.0) * 0.5; // Convert from [-1,1] to [0,1]
            vec3 skyColor = mix(bottomColor, topColor, t);
            
            // Sun effect
            float sunDot = dot(direction, sunPosition);
            float sunEffect = pow(max(sunDot, 0.0), 64.0);
            skyColor += sunColor * sunEffect * sunIntensity;
            
            // Horizon glow
            float horizonFactor = 1.0 - abs(direction.y);
            horizonFactor = pow(horizonFactor, 2.0);
            skyColor = mix(skyColor, vec3(1.0, 0.8, 0.6), horizonFactor * 0.1);
            
            FragColor = vec4(skyColor, 1.0);
        }
    )";
    
    m_skyShader->LoadFromStrings(vertexSource, fragmentSource);
}

void SkyBox::UpdateSkyColors() {
    // Update colors based on time of day
    float dayFactor = sin(m_timeOfDay * 3.14159f); // 0 at midnight, 1 at noon
    
    if (dayFactor > 0.0f) {
        // Day colors
        m_topColor = glm::mix(glm::vec3(0.2f, 0.2f, 0.4f), glm::vec3(0.5f, 0.7f, 1.0f), dayFactor);
        m_bottomColor = glm::mix(glm::vec3(0.1f, 0.1f, 0.2f), glm::vec3(0.8f, 0.9f, 1.0f), dayFactor);
        m_sunIntensity = dayFactor;
    } else {
        // Night colors
        m_topColor = glm::vec3(0.05f, 0.05f, 0.2f);
        m_bottomColor = glm::vec3(0.02f, 0.02f, 0.1f);
        m_sunIntensity = 0.0f;
    }
}

} // namespace FlightSim 
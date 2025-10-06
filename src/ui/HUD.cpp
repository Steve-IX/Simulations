#include "ui/HUD.h"
#include "physics/Aircraft.h"
#include "core/Camera.h"
#include "input/InputManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace FlightSim {

HUD::HUD()
    : m_enabled(true)
    , m_hudScale(1.0f)
    , m_hudColor(0.0f, 1.0f, 0.0f)
    , m_smoothedAltitude(0.0f)
    , m_smoothedSpeed(0.0f)
    , m_smoothedHeading(0.0f)
    , m_smoothedPitch(0.0f)
    , m_smoothedRoll(0.0f)
    , m_smoothedVerticalSpeed(0.0f)
    , m_hudAlpha(0.9f)
    , m_showDebugInfo(true)
    , m_inputManager(nullptr) {
}

HUD::~HUD() {
    Shutdown();
}

bool HUD::Initialize() {
    // Create HUD shader
    m_hudShader = std::make_unique<Shader>();
    
    std::string vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        uniform mat4 projection;
        uniform mat4 model;
        
        out vec2 TexCoord;
        
        void main() {
            gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    
    std::string fragmentSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec2 TexCoord;
        
        uniform vec3 hudColor;
        uniform float alpha;
        uniform bool useTexture;
        uniform sampler2D hudTexture;
        
        void main() {
            vec4 color = vec4(hudColor, alpha);
            
            if (useTexture) {
                vec4 texColor = texture(hudTexture, TexCoord);
                color = texColor * vec4(hudColor, alpha);
            }
            
            // Add subtle glow effect for better readability
            float glow = 0.2;
            vec3 glowColor = hudColor * (1.0 + glow);
            color.rgb = mix(color.rgb, glowColor, 0.3);
            
            FragColor = color;
        }
    )";
    
    if (!m_hudShader->LoadFromStrings(vertexSource, fragmentSource)) {
        std::cerr << "Failed to create HUD shader" << std::endl;
        return false;
    }
    
    // Setup basic HUD geometry
    SetupBuffers();
    
    return true;
}

void HUD::Shutdown() {
    m_hudShader.reset();
    
    // Simple cleanup without OpenGL calls for now
    m_VAO = 0;
    m_VBO = 0;
    m_EBO = 0;
}

void HUD::Update(const AircraftState& state, float deltaTime) {
    // Smooth instrument readings
    float smoothing = 0.95f;
    
    m_smoothedAltitude = glm::mix(m_smoothedAltitude, state.altitude, 1.0f - smoothing);
    m_smoothedSpeed = glm::mix(m_smoothedSpeed, state.airspeed, 1.0f - smoothing);
    m_smoothedHeading = glm::mix(m_smoothedHeading, state.heading, 1.0f - smoothing);
    m_smoothedPitch = glm::mix(m_smoothedPitch, state.pitch, 1.0f - smoothing);
    m_smoothedRoll = glm::mix(m_smoothedRoll, state.roll, 1.0f - smoothing);
    m_smoothedVerticalSpeed = glm::mix(m_smoothedVerticalSpeed, state.verticalSpeed, 1.0f - smoothing);
}

void HUD::Render(const Camera& camera, const AircraftState& state) {
    if (!m_enabled) return;
    
    // HUD rendering would go here in a full implementation
    // For now, this is a placeholder that doesn't use OpenGL directly
    
    // Render crosshair
    RenderCrosshair();
    
    // Render instruments
    RenderAltitudeIndicator(state);
    RenderSpeedIndicator(state);
    RenderArtificialHorizon(state);
    RenderHeadingIndicator(state);
    RenderVerticalSpeedIndicator(state);
    RenderEngineInstruments(state);
    RenderFlightInfo(state);
    
    // Render control indicators
    RenderControlIndicators(state);
}

void HUD::SetupBuffers() {
    // Simple buffer setup without OpenGL calls for now
    // In a full implementation, this would set up VAO/VBO for HUD rendering
    m_VAO = 0;
    m_VBO = 0;
    m_EBO = 0;
}

void HUD::RenderCrosshair() {
    // Simple crosshair rendering
    RenderLine(glm::vec2(640, 360 - 20), glm::vec2(640, 360 + 20), m_hudColor, 2.0f);
    RenderLine(glm::vec2(640 - 20, 360), glm::vec2(640 + 20, 360), m_hudColor, 2.0f);
}

void HUD::RenderAltitudeIndicator(const AircraftState& state) {
    // TODO: Implement altitude indicator with modern OpenGL
    // For now, this is a placeholder
}

void HUD::RenderSpeedIndicator(const AircraftState& state) {
    // TODO: Implement speed indicator with modern OpenGL
    // For now, this is a placeholder
}

void HUD::RenderArtificialHorizon(const AircraftState& state) {
    // TODO: Implement artificial horizon with modern OpenGL
    // For now, this is a placeholder
}

void HUD::RenderHeadingIndicator(const AircraftState& state) {
    // TODO: Implement heading indicator with modern OpenGL
    // For now, this is a placeholder
}

void HUD::RenderVerticalSpeedIndicator(const AircraftState& state) {
    // TODO: Implement vertical speed indicator with modern OpenGL
    // For now, this is a placeholder
}

void HUD::RenderEngineInstruments(const AircraftState& state) {
    // TODO: Implement engine instruments with modern OpenGL
    // For now, this is a placeholder
}

void HUD::RenderFlightInfo(const AircraftState& state) {
    // TODO: Implement flight info display with modern OpenGL
    // For now, this is a placeholder
}

void HUD::RenderControlIndicators(const AircraftState& state) {
    if (!m_enabled || !m_inputManager) return;
    
    // Get current control inputs
    ControlInputs controls = m_inputManager->GetControlInputs();
    
    // Control panel background (bottom right)
    glm::vec2 panelPos(950.0f, 500.0f);
    glm::vec2 panelSize(300.0f, 200.0f);
    
    // For now, we'll store the control values for display
    // In a full implementation, this would render the control panel with OpenGL
    
    // The actual visual rendering will be handled in the main HUD render function
    // This function prepares the control data for display
}

void HUD::RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    // TODO: Implement text rendering with modern OpenGL
    // For now, this is a placeholder
}

void HUD::RenderLine(const glm::vec2& start, const glm::vec2& end, const glm::vec3& color, float width) {
    // TODO: Implement line rendering with modern OpenGL
    // For now, this is a placeholder
}

void HUD::RenderQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color) {
    // TODO: Implement quad rendering with modern OpenGL
    // For now, this is a placeholder
}

void HUD::RenderCircle(const glm::vec2& center, float radius, const glm::vec3& color, int segments) {
    // TODO: Implement circle rendering with modern OpenGL
    // For now, this is a placeholder
}

} // namespace FlightSim 
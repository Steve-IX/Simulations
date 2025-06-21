#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "../physics/Aircraft.h"
#include "core/Shader.h"

namespace FlightSim {

struct AircraftState;
class Camera;

class HUD {
public:
    HUD();
    ~HUD();
    
    bool Initialize();
    void Shutdown();
    
    void Update(const AircraftState& state, float deltaTime);
    void Render(const Camera& camera, const AircraftState& state);
    
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }
    
private:
    void SetupBuffers();
    void RenderCrosshair();
    void RenderAltitudeIndicator(const AircraftState& state);
    void RenderSpeedIndicator(const AircraftState& state);
    void RenderArtificialHorizon(const AircraftState& state);
    void RenderHeadingIndicator(const AircraftState& state);
    void RenderVerticalSpeedIndicator(const AircraftState& state);
    void RenderEngineInstruments(const AircraftState& state);
    void RenderFlightInfo(const AircraftState& state);
    
    // Rendering helpers
    void RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color);
    void RenderLine(const glm::vec2& start, const glm::vec2& end, const glm::vec3& color, float width = 1.0f);
    void RenderQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color);
    void RenderCircle(const glm::vec2& center, float radius, const glm::vec3& color, int segments = 32);
    
    std::unique_ptr<Shader> m_hudShader;
    std::unique_ptr<Shader> m_textShader;
    
    // OpenGL objects
    unsigned int m_VAO, m_VBO;
    unsigned int m_fontTexture;
    
    // HUD settings
    bool m_enabled;
    float m_hudScale;
    glm::vec3 m_hudColor;
    
    // Instrument positions (normalized screen coordinates)
    struct InstrumentLayout {
        glm::vec2 altitudePos{0.9f, 0.5f};
        glm::vec2 speedPos{0.1f, 0.5f};
        glm::vec2 horizonPos{0.5f, 0.5f};
        glm::vec2 headingPos{0.5f, 0.1f};
        glm::vec2 verticalSpeedPos{0.95f, 0.5f};
        glm::vec2 enginePos{0.1f, 0.9f};
        glm::vec2 infoPos{0.1f, 0.1f};
    } m_layout;
    
    // Animation/smoothing
    float m_smoothedAltitude;
    float m_smoothedSpeed;
    float m_smoothedHeading;
    float m_smoothedPitch;
    float m_smoothedRoll;
};

} // namespace FlightSim 
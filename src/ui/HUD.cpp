#include "ui/HUD.h"
#include "core/Camera.h"
#include "core/Shader.h"
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace FlightSim {

HUD::HUD()
    : m_hudShader(nullptr)
    , m_VAO(0)
    , m_VBO(0)
    , m_smoothedAltitude(0.0f)
    , m_smoothedSpeed(0.0f)
    , m_smoothedVerticalSpeed(0.0f)
    , m_smoothedHeading(0.0f)
    , m_smoothedRoll(0.0f)
    , m_hudColor(0.0f, 1.0f, 0.0f) // Bright green for visibility
    , m_hudAlpha(0.9f)
    , m_showDebugInfo(true) {
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
        
        out vec2 TexCoord;
        
        uniform mat4 projection;
        
        void main() {
            TexCoord = aTexCoord;
            gl_Position = projection * vec4(aPos, 0.0, 1.0);
        }
    )";
    
    std::string fragmentSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec2 TexCoord;
        
        uniform vec3 hudColor;
        uniform float alpha;
        
        void main() {
            FragColor = vec4(hudColor, alpha);
        }
    )";
    
    if (!m_hudShader->LoadFromStrings(vertexSource, fragmentSource)) {
        std::cerr << "Failed to load HUD shader" << std::endl;
        return false;
    }
    
    SetupBuffers();
    return true;
}

void HUD::SetupBuffers() {
    // Create VAO and VBO for HUD elements
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0);
}

void HUD::Update(const AircraftState& state, float deltaTime) {
    // Smooth the values for better visual display
    float smoothing = 0.1f;
    m_smoothedAltitude = glm::mix(m_smoothedAltitude, state.altitude, smoothing);
    m_smoothedSpeed = glm::mix(m_smoothedSpeed, state.airspeed, smoothing);
    m_smoothedVerticalSpeed = glm::mix(m_smoothedVerticalSpeed, state.verticalSpeed, smoothing);
    m_smoothedHeading = glm::mix(m_smoothedHeading, state.heading, smoothing);
    m_smoothedRoll = glm::mix(m_smoothedRoll, state.roll, smoothing);
    
    // Update HUD color based on aircraft state
    if (state.airspeed > 100.0f) {
        m_hudColor = glm::vec3(1.0f, 0.0f, 0.0f); // Red for high speed
    } else if (state.altitude > 2000.0f) {
        m_hudColor = glm::vec3(0.0f, 1.0f, 1.0f); // Cyan for high altitude
    } else {
        m_hudColor = glm::vec3(0.0f, 1.0f, 0.0f); // Green for normal flight
    }
}

void HUD::Render(const Camera& camera, const AircraftState& state) {
    if (!m_hudShader) return;
    
    // Set up orthographic projection for 2D HUD
    glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);
    
    m_hudShader->Use();
    m_hudShader->SetMat4("projection", projection);
    m_hudShader->SetVec3("hudColor", m_hudColor);
    m_hudShader->SetFloat("alpha", m_hudAlpha);
    
    glBindVertexArray(m_VAO);
    
    // Render crosshair
    RenderCrosshair();
    
    // Render primary flight instruments
    RenderAltitudeIndicator(state);
    RenderSpeedIndicator(state);
    RenderArtificialHorizon(state);
    RenderHeadingIndicator(state);
    RenderVerticalSpeedIndicator(state);
    
    // Render engine instruments
    RenderEngineInstruments(state);
    
    // Render flight information
    RenderFlightInfo(state);
    
    // Render control input indicators
    RenderControlIndicators(state);
    
    glBindVertexArray(0);
    m_hudShader->Unbind();
}

void HUD::RenderCrosshair() {
    // Render a clear crosshair in the center
    float centerX = 1280.0f / 2.0f;
    float centerY = 720.0f / 2.0f;
    float size = 20.0f;
    float thickness = 2.0f;
    
    // Horizontal line
    RenderLine(glm::vec2(centerX - size, centerY), 
               glm::vec2(centerX + size, centerY), 
               glm::vec3(1.0f, 1.0f, 1.0f), thickness);
    
    // Vertical line
    RenderLine(glm::vec2(centerX, centerY - size), 
               glm::vec2(centerX, centerY + size), 
               glm::vec3(1.0f, 1.0f, 1.0f), thickness);
    
    // Center dot
    RenderCircle(glm::vec2(centerX, centerY), 3.0f, glm::vec3(1.0f, 1.0f, 1.0f), 16);
}

void HUD::RenderAltitudeIndicator(const AircraftState& state) {
    float x = 50.0f;
    float y = 200.0f;
    float width = 100.0f;
    float height = 300.0f;
    
    // Background
    RenderQuad(glm::vec2(x, y), glm::vec2(width, height), glm::vec3(0.0f, 0.0f, 0.0f));
    
    // Altitude tape
    float alt = m_smoothedAltitude;
    float centerY = y + height / 2.0f;
    float pixelsPerMeter = 2.0f;
    
    // Draw altitude marks
    for (int i = -10; i <= 10; i++) {
        float markAlt = alt + i * 100.0f;
        float markY = centerY - (markAlt - alt) * pixelsPerMeter;
        
        if (markY >= y && markY <= y + height) {
            float markWidth = (i % 5 == 0) ? 60.0f : 30.0f;
            RenderLine(glm::vec2(x + width - markWidth, markY), 
                      glm::vec2(x + width, markY), 
                      glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
            
            if (i % 5 == 0) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(0) << markAlt;
                RenderText(ss.str(), x + width - markWidth - 30, markY - 5, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            }
        }
    }
    
    // Current altitude indicator
    RenderLine(glm::vec2(x + width - 80, centerY), 
               glm::vec2(x + width, centerY), 
               glm::vec3(1.0f, 0.0f, 0.0f), 3.0f);
}

void HUD::RenderSpeedIndicator(const AircraftState& state) {
    float x = 1130.0f;
    float y = 200.0f;
    float width = 100.0f;
    float height = 300.0f;
    
    // Background
    RenderQuad(glm::vec2(x, y), glm::vec2(width, height), glm::vec3(0.0f, 0.0f, 0.0f));
    
    // Speed tape
    float speed = m_smoothedSpeed;
    float centerY = y + height / 2.0f;
    float pixelsPerKnot = 3.0f;
    
    // Draw speed marks
    for (int i = -10; i <= 10; i++) {
        float markSpeed = speed + i * 10.0f;
        float markY = centerY - (markSpeed - speed) * pixelsPerKnot;
        
        if (markY >= y && markY <= y + height) {
            float markWidth = (i % 5 == 0) ? 60.0f : 30.0f;
            RenderLine(glm::vec2(x, markY), 
                      glm::vec2(x + markWidth, markY), 
                      glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
            
            if (i % 5 == 0) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(0) << markSpeed;
                RenderText(ss.str(), x + markWidth + 5, markY - 5, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            }
        }
    }
    
    // Current speed indicator
    RenderLine(glm::vec2(x, centerY), 
               glm::vec2(x + 80, centerY), 
               glm::vec3(1.0f, 0.0f, 0.0f), 3.0f);
}

void HUD::RenderArtificialHorizon(const AircraftState& state) {
    float centerX = 1280.0f / 2.0f;
    float centerY = 720.0f / 2.0f;
    float radius = 80.0f;
    
    // Background circle
    RenderCircle(glm::vec2(centerX, centerY), radius, glm::vec3(0.0f, 0.0f, 0.0f), 32);
    
    // Horizon line
    float roll = glm::radians(m_smoothedRoll);
    float pitch = glm::radians(state.pitch);
    
    float pitchOffset = pitch * 50.0f; // Scale pitch to pixels
    
    glm::vec2 start(centerX - radius * cos(roll), centerY + pitchOffset - radius * sin(roll));
    glm::vec2 end(centerX + radius * cos(roll), centerY + pitchOffset + radius * sin(roll));
    
    RenderLine(start, end, glm::vec3(1.0f, 1.0f, 1.0f), 3.0f);
    
    // Center reference
    RenderCircle(glm::vec2(centerX, centerY), 5.0f, glm::vec3(1.0f, 0.0f, 0.0f), 16);
}

void HUD::RenderHeadingIndicator(const AircraftState& state) {
    float centerX = 1280.0f / 2.0f;
    float y = 600.0f;
    float width = 200.0f;
    float height = 40.0f;
    
    // Background
    RenderQuad(glm::vec2(centerX - width/2, y), glm::vec2(width, height), glm::vec3(0.0f, 0.0f, 0.0f));
    
    // Heading marks
    float heading = m_smoothedHeading;
    float centerX_actual = centerX;
    float pixelsPerDegree = 2.0f;
    
    for (int i = -5; i <= 5; i++) {
        float markHeading = fmod(heading + i * 30.0f + 360.0f, 360.0f);
        float markX = centerX_actual + (markHeading - heading) * pixelsPerDegree;
        
        if (markX >= centerX - width/2 && markX <= centerX + width/2) {
            float markHeight = (i % 3 == 0) ? 30.0f : 15.0f;
            RenderLine(glm::vec2(markX, y + height - markHeight), 
                      glm::vec2(markX, y + height), 
                      glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
            
            if (i % 3 == 0) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(0) << markHeading;
                RenderText(ss.str(), markX - 10, y + height - markHeight - 15, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));
            }
        }
    }
    
    // Current heading indicator
    RenderLine(glm::vec2(centerX_actual, y), 
               glm::vec2(centerX_actual, y + height), 
               glm::vec3(1.0f, 0.0f, 0.0f), 3.0f);
}

void HUD::RenderVerticalSpeedIndicator(const AircraftState& state) {
    float x = 1130.0f;
    float y = 520.0f;
    float width = 60.0f;
    float height = 120.0f;
    
    // Background
    RenderQuad(glm::vec2(x, y), glm::vec2(width, height), glm::vec3(0.0f, 0.0f, 0.0f));
    
    // Vertical speed indicator
    float vspeed = m_smoothedVerticalSpeed;
    float centerY = y + height / 2.0f;
    float pixelsPerFPS = 2.0f;
    
    // Draw marks
    for (int i = -5; i <= 5; i++) {
        float markVSpeed = vspeed + i * 500.0f;
        float markY = centerY - (markVSpeed - vspeed) * pixelsPerFPS / 1000.0f;
        
        if (markY >= y && markY <= y + height) {
            float markWidth = (i % 2 == 0) ? 40.0f : 20.0f;
            RenderLine(glm::vec2(x + width - markWidth, markY), 
                      glm::vec2(x + width, markY), 
                      glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
        }
    }
    
    // Current vertical speed indicator
    RenderLine(glm::vec2(x + width - 50, centerY), 
               glm::vec2(x + width, centerY), 
               glm::vec3(1.0f, 0.0f, 0.0f), 3.0f);
}

void HUD::RenderEngineInstruments(const AircraftState& state) {
    float x = 50.0f;
    float y = 520.0f;
    float width = 200.0f;
    float height = 120.0f;
    
    // Background
    RenderQuad(glm::vec2(x, y), glm::vec2(width, height), glm::vec3(0.0f, 0.0f, 0.0f));
    
    // Throttle indicator
    float throttle = state.throttle * 100.0f;
    std::stringstream ss;
    ss << "THROTTLE: " << std::fixed << std::setprecision(0) << throttle << "%";
    RenderText(ss.str(), x + 10, y + 20, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
    
    // Throttle bar
    float barWidth = width - 20;
    float barHeight = 20;
    RenderQuad(glm::vec2(x + 10, y + 40), glm::vec2(barWidth, barHeight), glm::vec3(0.3f, 0.3f, 0.3f));
    RenderQuad(glm::vec2(x + 10, y + 40), glm::vec2(barWidth * throttle / 100.0f, barHeight), glm::vec3(0.0f, 1.0f, 0.0f));
}

void HUD::RenderFlightInfo(const AircraftState& state) {
    float x = 50.0f;
    float y = 50.0f;
    
    // Flight information panel
    RenderQuad(glm::vec2(x, y), glm::vec2(300, 120), glm::vec3(0.0f, 0.0f, 0.0f));
    
    std::stringstream ss;
    ss << "ALT: " << std::fixed << std::setprecision(0) << m_smoothedAltitude << " ft";
    RenderText(ss.str(), x + 10, y + 20, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
    
    ss.str("");
    ss << "SPD: " << std::fixed << std::setprecision(0) << m_smoothedSpeed << " kts";
    RenderText(ss.str(), x + 10, y + 40, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
    
    ss.str("");
    ss << "HDG: " << std::fixed << std::setprecision(0) << m_smoothedHeading << "°";
    RenderText(ss.str(), x + 10, y + 60, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
    
    ss.str("");
    ss << "VS: " << std::fixed << std::setprecision(0) << m_smoothedVerticalSpeed << " fpm";
    RenderText(ss.str(), x + 10, y + 80, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void HUD::RenderControlIndicators(const AircraftState& state) {
    float x = 1130.0f;
    float y = 50.0f;
    
    // Control input panel
    RenderQuad(glm::vec2(x, y), glm::vec2(150, 120), glm::vec3(0.0f, 0.0f, 0.0f));
    
    RenderText("CONTROLS", x + 10, y + 20, 0.6f, glm::vec3(1.0f, 1.0f, 0.0f));
    RenderText("W/S: Pitch", x + 10, y + 40, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText("A/D: Roll", x + 10, y + 55, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText("Q/E: Yaw", x + 10, y + 70, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText("Shift/Ctrl: Throttle", x + 10, y + 85, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText("C: Camera", x + 10, y + 100, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void HUD::RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    // Simple text rendering using OpenGL immediate mode
    // In a real implementation, you'd use a proper text rendering library
    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1280, 720, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glColor3f(color.r, color.g, color.b);
    glRasterPos2f(x, y);
    
    // For now, just render a simple rectangle to represent text
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + text.length() * 8 * scale, y);
    glVertex2f(x + text.length() * 8 * scale, y + 15 * scale);
    glVertex2f(x, y + 15 * scale);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
}

void HUD::RenderLine(const glm::vec2& start, const glm::vec2& end, const glm::vec3& color, float width) {
    // Simple line rendering
    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1280, 720, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glColor3f(color.r, color.g, color.b);
    glLineWidth(width);
    
    glBegin(GL_LINES);
    glVertex2f(start.x, start.y);
    glVertex2f(end.x, end.y);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
}

void HUD::RenderQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color) {
    // Simple quad rendering
    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1280, 720, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glColor3f(color.r, color.g, color.b);
    
    glBegin(GL_QUADS);
    glVertex2f(position.x, position.y);
    glVertex2f(position.x + size.x, position.y);
    glVertex2f(position.x + size.x, position.y + size.y);
    glVertex2f(position.x, position.y + size.y);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
}

void HUD::RenderCircle(const glm::vec2& center, float radius, const glm::vec3& color, int segments) {
    // Simple circle rendering
    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1280, 720, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glColor3f(color.r, color.g, color.b);
    
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        float x = center.x + radius * cos(angle);
        float y = center.y + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
}

void HUD::Shutdown() {
    if (m_VAO) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    m_hudShader.reset();
}

} // namespace FlightSim 
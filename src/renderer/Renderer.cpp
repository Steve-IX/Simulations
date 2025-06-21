#include "renderer/Renderer.h"
#include "physics/Aircraft.h"
#include "core/Mesh.h"
#include "core/Shader.h"
#include "renderer/SkyBox.h"
#include "renderer/Terrain.h"
#include <glad/glad.h>
#include <iostream>

namespace FlightSim {

Renderer::Renderer()
    : m_aircraftShader(nullptr)
    , m_skybox(nullptr)
    , m_terrain(nullptr)
    , m_aircraftMesh(nullptr)
    , m_initialized(false)
    , m_directionalLightDir(0.3f, -1.0f, 0.2f)
    , m_directionalLightColor(1.0f, 1.0f, 0.9f)
    , m_ambientLightColor(0.2f, 0.2f, 0.3f)
    , m_fogDensity(0.00005f)
    , m_fogColor(0.7f, 0.8f, 0.9f)
    , m_wireframeMode(false)
    , m_showInstruments(true)
    , m_frameCount(0)
    , m_lastFPSUpdate(0.0f)
    , m_currentFPS(0.0f) {
}

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Enable OpenGL features
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    
    // Create shaders
    m_aircraftShader = std::make_unique<Shader>();
    if (!m_aircraftShader->LoadFromFiles("resources/shaders/aircraft.vert", "resources/shaders/aircraft.frag")) {
        std::cerr << "Failed to load aircraft shader" << std::endl;
        return false;
    }
    
    // Create meshes
    m_aircraftMesh = std::make_unique<Mesh>();
    m_aircraftMesh->CreateAircraft();
    
    // Create skybox
    m_skybox = std::make_unique<SkyBox>();
    if (!m_skybox->Initialize()) {
        std::cerr << "Failed to initialize skybox" << std::endl;
        return false;
    }
    
    // Create terrain
    m_terrain = std::make_unique<Terrain>();
    if (!m_terrain->Initialize()) {
        std::cerr << "Failed to initialize terrain" << std::endl;
        return false;
    }
    
    // Set up lighting
    m_lightPosition = glm::vec3(1000.0f, 1000.0f, 1000.0f);
    m_lightColor = glm::vec3(1.0f, 0.95f, 0.8f); // Warm sunlight
    m_ambientStrength = 0.3f;
    m_diffuseStrength = 0.7f;
    m_specularStrength = 0.5f;
    
    m_initialized = true;
    return true;
}

void Renderer::Shutdown() {
    m_aircraftShader.reset();
    m_skybox.reset();
    m_terrain.reset();
    m_aircraftMesh.reset();
    m_initialized = false;
}

void Renderer::BeginFrame() {
    glClearColor(0.2f, 0.3f, 0.5f, 1.0f); // Sky blue background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (m_wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Renderer::EndFrame() {
    m_frameCount++;
}

void Renderer::RenderScene(const Camera& camera, const Aircraft& aircraft) {
    if (!m_initialized) return;
    
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.GetProjectionMatrix();
    
    // Render skybox
    m_skybox->Render(view, projection);
    
    // Render terrain
    m_terrain->Render(view, projection, m_lightPosition, m_lightColor);
    
    // Render aircraft with enhanced visuals
    RenderAircraft(camera, aircraft, view, projection);
    
    // Render flight path trail
    RenderFlightPath(aircraft, view, projection);
    
    // Render ground reference grid
    RenderGroundGrid(view, projection);
}

void Renderer::SetViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

void Renderer::SetDirectionalLight(const glm::vec3& direction, const glm::vec3& color) {
    m_directionalLightDir = direction;
    m_directionalLightColor = color;
}

void Renderer::SetAmbientLight(const glm::vec3& color) {
    m_ambientLightColor = color;
}

void Renderer::SetFog(float density, const glm::vec3& color) {
    m_fogDensity = density;
    m_fogColor = color;
}

void Renderer::RenderAircraft(const Camera& camera, const Aircraft& aircraft, 
                             const glm::mat4& view, const glm::mat4& projection) {
    const AircraftState& state = aircraft.GetState();
    glm::mat4 model = aircraft.GetModelMatrix();
    
    m_aircraftShader->Use();
    
    // Set matrices
    m_aircraftShader->SetMat4("model", model);
    m_aircraftShader->SetMat4("view", view);
    m_aircraftShader->SetMat4("projection", projection);
    
    // Set lighting
    m_aircraftShader->SetVec3("lightPos", m_lightPosition);
    m_aircraftShader->SetVec3("lightColor", m_lightColor);
    m_aircraftShader->SetVec3("viewPos", camera.GetPosition());
    m_aircraftShader->SetFloat("ambientStrength", m_ambientStrength);
    m_aircraftShader->SetFloat("diffuseStrength", m_diffuseStrength);
    m_aircraftShader->SetFloat("specularStrength", m_specularStrength);
    
    // Set material properties for better visibility
    m_aircraftShader->SetVec3("material.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    m_aircraftShader->SetVec3("material.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    m_aircraftShader->SetVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    m_aircraftShader->SetFloat("material.shininess", 32.0f);
    
    // Add aircraft color based on state
    glm::vec3 aircraftColor = glm::vec3(0.7f, 0.7f, 0.9f); // Default blue-gray
    if (state.airspeed > 50.0f) {
        aircraftColor = glm::vec3(0.9f, 0.7f, 0.7f); // Red tint at high speed
    } else if (state.altitude > 1000.0f) {
        aircraftColor = glm::vec3(0.7f, 0.9f, 0.7f); // Green tint at high altitude
    }
    m_aircraftShader->SetVec3("aircraftColor", aircraftColor);
    
    // Render aircraft mesh
    m_aircraftMesh->Render();
    
    // Render aircraft orientation indicators
    RenderOrientationIndicators(aircraft, view, projection);
    
    m_aircraftShader->Unbind();
}

void Renderer::RenderOrientationIndicators(const Aircraft& aircraft, 
                                          const glm::mat4& view, const glm::mat4& projection) {
    const AircraftState& state = aircraft.GetState();
    glm::vec3 position = state.position;
    
    // Render forward direction indicator (red arrow)
    glm::vec3 forward = aircraft.GetForward();
    RenderArrow(position, position + forward * 10.0f, glm::vec3(1.0f, 0.0f, 0.0f), view, projection);
    
    // Render up direction indicator (green arrow)
    glm::vec3 up = aircraft.GetUp();
    RenderArrow(position, position + up * 5.0f, glm::vec3(0.0f, 1.0f, 0.0f), view, projection);
    
    // Render right direction indicator (blue arrow)
    glm::vec3 right = aircraft.GetRight();
    RenderArrow(position, position + right * 5.0f, glm::vec3(0.0f, 0.0f, 1.0f), view, projection);
}

void Renderer::RenderArrow(const glm::vec3& start, const glm::vec3& end, 
                          const glm::vec3& color, const glm::mat4& view, const glm::mat4& projection) {
    // Simple line rendering for direction indicators
    glUseProgram(0); // Use fixed function pipeline for simple lines
    
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projection));
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(view));
    
    glBegin(GL_LINES);
    glColor3f(color.r, color.g, color.b);
    glVertex3f(start.x, start.y, start.z);
    glVertex3f(end.x, end.y, end.z);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f); // Reset color
}

void Renderer::RenderFlightPath(const Aircraft& aircraft, 
                               const glm::mat4& view, const glm::mat4& projection) {
    // Render a simple flight path trail
    static std::vector<glm::vec3> trail;
    static int trailIndex = 0;
    
    const AircraftState& state = aircraft.GetState();
    
    // Add current position to trail every few frames
    if (trailIndex % 10 == 0) {
        trail.push_back(state.position);
        if (trail.size() > 100) { // Keep trail length manageable
            trail.erase(trail.begin());
        }
    }
    trailIndex++;
    
    // Render trail
    if (trail.size() > 1) {
        glUseProgram(0);
        
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(projection));
        
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(view));
        
        glBegin(GL_LINE_STRIP);
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow trail
        for (const auto& point : trail) {
            glVertex3f(point.x, point.y, point.z);
        }
        glEnd();
        
        glColor3f(1.0f, 1.0f, 1.0f);
    }
}

void Renderer::RenderGroundGrid(const glm::mat4& view, const glm::mat4& projection) {
    // Render a reference grid on the ground
    glUseProgram(0);
    
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projection));
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(view));
    
    glBegin(GL_LINES);
    glColor3f(0.5f, 0.5f, 0.5f); // Gray grid
    
    // Draw grid lines
    for (int i = -50; i <= 50; i += 10) {
        // X lines
        glVertex3f(i * 10.0f, 0.0f, -500.0f);
        glVertex3f(i * 10.0f, 0.0f, 500.0f);
        
        // Z lines
        glVertex3f(-500.0f, 0.0f, i * 10.0f);
        glVertex3f(500.0f, 0.0f, i * 10.0f);
    }
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
}

void Renderer::RenderInstruments(const Aircraft& aircraft) {
    // This would render additional instrument overlays
    // For now, it's handled by the HUD class
}

} // namespace FlightSim 
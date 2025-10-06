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
    , m_terrainShader(nullptr)
    , m_hudShader(nullptr)
    , m_skybox(nullptr)
    , m_terrain(nullptr)
    , m_aircraftMesh(nullptr)
    , m_directionalLightDir(0.5f, 1.0f, 0.3f)
    , m_directionalLightColor(1.0f, 0.95f, 0.8f)
    , m_ambientLightColor(0.3f, 0.3f, 0.4f)
    , m_fogDensity(0.00005f)
    , m_fogColor(0.7f, 0.8f, 0.9f)
    , m_wireframeMode(false)
    , m_showInstruments(true)
    , m_frameCount(0)
    , m_lastFPSUpdate(0.0f)
    , m_currentFPS(0.0f)
    , m_initialized(false) {
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
    *m_aircraftMesh = Mesh::CreateAircraft();
    m_aircraftMesh->Upload();
    
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
    
    // Initialize lighting (already set in constructor)
    
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
    m_skybox->Render(camera);
    
    // Render terrain
    m_terrain->Render(camera);
    
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
    // Calculate light position from directional light
    glm::vec3 lightPos = -m_directionalLightDir * 1000.0f;
    m_aircraftShader->SetVec3("lightPos", lightPos);
    m_aircraftShader->SetVec3("lightColor", m_directionalLightColor);
    m_aircraftShader->SetVec3("viewPos", camera.GetPosition());
    m_aircraftShader->SetFloat("ambientStrength", 0.3f);
    m_aircraftShader->SetFloat("diffuseStrength", 0.7f);
    m_aircraftShader->SetFloat("specularStrength", 0.5f);
    
    // Set material properties for better visibility
    m_aircraftShader->SetVec3("material_ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    m_aircraftShader->SetVec3("material_diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    m_aircraftShader->SetVec3("material_specular", glm::vec3(1.0f, 1.0f, 1.0f));
    m_aircraftShader->SetFloat("material_shininess", 32.0f);
    
    // Add aircraft color based on state
    glm::vec3 aircraftColor = glm::vec3(0.7f, 0.7f, 0.9f); // Default blue-gray
    if (state.airspeed > 50.0f) {
        aircraftColor = glm::vec3(0.9f, 0.7f, 0.7f); // Red tint at high speed
    } else if (state.altitude > 1000.0f) {
        aircraftColor = glm::vec3(0.7f, 0.9f, 0.7f); // Green tint at high altitude
    }
    m_aircraftShader->SetVec3("aircraftColor", aircraftColor);
    
    // Set fog uniforms
    m_aircraftShader->SetFloat("fogDensity", m_fogDensity);
    m_aircraftShader->SetVec3("fogColor", m_fogColor);
    
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
    // TODO: Implement with modern OpenGL using VAO/VBO
    // For now, this is a placeholder - arrows can be rendered as part of the HUD
}

void Renderer::RenderFlightPath(const Aircraft& aircraft, 
                               const glm::mat4& view, const glm::mat4& projection) {
    // TODO: Implement flight path trail with modern OpenGL
    // For now, this is a placeholder
}

void Renderer::RenderGroundGrid(const glm::mat4& view, const glm::mat4& projection) {
    // TODO: Implement ground grid with modern OpenGL
    // For now, this is a placeholder
}

void Renderer::RenderInstruments(const Aircraft& aircraft) {
    // This would render additional instrument overlays
    // For now, it's handled by the HUD class
}

} // namespace FlightSim 
#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "../core/Shader.h"
#include "../core/Camera.h"
#include "SkyBox.h"
#include "Terrain.h"

namespace FlightSim {

class Aircraft;
class Mesh;

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool Initialize();
    void Shutdown();
    
    void BeginFrame();
    void EndFrame();
    
    void RenderScene(const Camera& camera, const Aircraft& aircraft);
    void SetViewport(int width, int height);
    
    // Lighting
    void SetDirectionalLight(const glm::vec3& direction, const glm::vec3& color);
    void SetAmbientLight(const glm::vec3& color);
    
    // Fog settings for atmosphere
    void SetFog(float density, const glm::vec3& color);
    
    void RenderInstruments(const Aircraft& aircraft);
    
private:
    void SetupOpenGL();
    void RenderAircraft(const Camera& camera, const Aircraft& aircraft, 
                       const glm::mat4& view, const glm::mat4& projection);
    void RenderOrientationIndicators(const Aircraft& aircraft, 
                                    const glm::mat4& view, const glm::mat4& projection);
    void RenderArrow(const glm::vec3& start, const glm::vec3& end, 
                     const glm::vec3& color, const glm::mat4& view, const glm::mat4& projection);
    void RenderFlightPath(const Aircraft& aircraft, 
                         const glm::mat4& view, const glm::mat4& projection);
    void RenderGroundGrid(const glm::mat4& view, const glm::mat4& projection);
    
    // Shaders
    std::unique_ptr<Shader> m_aircraftShader;
    std::unique_ptr<Shader> m_terrainShader;
    std::unique_ptr<Shader> m_hudShader;
    
    // Scene objects
    std::unique_ptr<SkyBox> m_skybox;
    std::unique_ptr<Terrain> m_terrain;
    std::unique_ptr<Mesh> m_aircraftMesh;
    
    // Lighting
    glm::vec3 m_directionalLightDir;
    glm::vec3 m_directionalLightColor;
    glm::vec3 m_ambientLightColor;
    
    // Fog
    float m_fogDensity;
    glm::vec3 m_fogColor;
    
    // Render settings
    bool m_wireframeMode;
    bool m_showInstruments;
    
    // Performance tracking
    int m_frameCount;
    float m_lastFPSUpdate;
    float m_currentFPS;
};

} // namespace FlightSim 
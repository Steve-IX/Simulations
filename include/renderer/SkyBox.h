#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "../core/Shader.h"
#include "../core/Mesh.h"

namespace FlightSim {

class Camera;

class SkyBox {
public:
    SkyBox();
    ~SkyBox();
    
    bool Initialize();
    void Shutdown();
    
    void Render(const Camera& camera);
    
    // Sky settings
    void SetSkyColor(const glm::vec3& topColor, const glm::vec3& bottomColor);
    void SetSunPosition(const glm::vec3& position);
    void SetTimeOfDay(float time); // 0.0 = midnight, 0.5 = noon, 1.0 = midnight
    
private:
    void CreateSkyMesh();
    void SetupShaders();
    void UpdateSkyColors();
    
    std::unique_ptr<Shader> m_skyShader;
    std::unique_ptr<Mesh> m_skyMesh;
    
    // Sky properties
    glm::vec3 m_topColor;
    glm::vec3 m_bottomColor;
    glm::vec3 m_sunPosition;
    float m_timeOfDay;
    
    // Atmospheric parameters
    float m_sunIntensity;
    glm::vec3 m_sunColor;
    glm::vec3 m_horizonColor;
};

} // namespace FlightSim 
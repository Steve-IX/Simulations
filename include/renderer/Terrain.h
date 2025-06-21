#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "../core/Shader.h"
#include "../core/Mesh.h"

namespace FlightSim {

class Camera;

class Terrain {
public:
    Terrain();
    ~Terrain();
    
    bool Initialize();
    void Shutdown();
    
    void Render(const Camera& camera);
    
    // Terrain generation
    void GenerateTerrain(int width, int height, float scale = 1.0f);
    float GetHeightAt(float x, float z) const;
    
    // Settings
    void SetTerrainColor(const glm::vec3& color) { m_terrainColor = color; }
    void SetGridSize(int size) { m_gridSize = size; }
    
private:
    void CreateTerrainMesh();
    void SetupShaders();
    
    std::unique_ptr<Shader> m_terrainShader;
    std::unique_ptr<Mesh> m_terrainMesh;
    
    // Terrain properties
    int m_gridSize;
    float m_terrainScale;
    glm::vec3 m_terrainColor;
    
    // Height data
    std::vector<std::vector<float>> m_heightMap;
    int m_terrainWidth;
    int m_terrainHeight;
};

} // namespace FlightSim 
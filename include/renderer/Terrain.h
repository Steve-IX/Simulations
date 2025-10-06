#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <unordered_map>

namespace FlightSim {

class Camera;
class Mesh;
class Shader;

// MSFS-Style Infinite Terrain System
class Terrain {
public:
    Terrain();
    ~Terrain();
    
    bool Initialize();
    void Shutdown();
    void Render(const Camera& camera);
    
    // Terrain queries
    float GetHeightAt(float x, float z) const;
    glm::vec3 GetNormalAt(float x, float z) const;
    
    // Configuration
    void SetTerrainColor(const glm::vec3& color) { m_terrainColor = color; }
    void SetRenderDistance(int distance) { m_renderDistance = distance; }
    
private:
    struct TerrainChunk {
        glm::vec2 position;
        std::unique_ptr<Mesh> mesh;
        bool needsUpdate;
    };
    
    void CreateTerrainMesh();
    void SetupShaders();
    void GenerateChunk(const glm::vec2& chunkPos, TerrainChunk& chunk);
    float GenerateHeight(float x, float z) const;
    
    // Chunk management
    int64_t GetChunkKey(int x, int z) const;
    void UpdateChunks(const glm::vec3& cameraPos);
    
    std::unique_ptr<Shader> m_terrainShader;
    std::unique_ptr<Mesh> m_baseMesh;
    
    // Chunk system for infinite terrain
    std::unordered_map<int64_t, TerrainChunk> m_chunks;
    int m_chunkSize;
    int m_renderDistance;
    int m_gridResolution;
    
    glm::vec3 m_terrainColor;
    glm::vec2 m_lastCameraChunk;
};

} // namespace FlightSim

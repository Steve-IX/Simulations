#include "renderer/Terrain.h"
#include "core/Camera.h"
#include "core/Mesh.h"
#include "core/Shader.h"
#include <glad/glad.h>
#include <cmath>
#include <iostream>

namespace FlightSim {

Terrain::Terrain()
    : m_chunkSize(2000)
    , m_renderDistance(10)
    , m_gridResolution(50)
    , m_terrainColor(0.35f, 0.55f, 0.3f)
    , m_lastCameraChunk(-999999.0f, -999999.0f) {
}

Terrain::~Terrain() {
    Shutdown();
}

bool Terrain::Initialize() {
    SetupShaders();
    CreateTerrainMesh();
    return true;
}

void Terrain::Shutdown() {
    m_chunks.clear();
    m_baseMesh.reset();
    m_terrainShader.reset();
}

void Terrain::Render(const Camera& camera) {
    if (!m_terrainShader || !m_baseMesh) return;
    
    glm::vec3 cameraPos = camera.GetPosition();
    UpdateChunks(cameraPos);
    
    m_terrainShader->Use();
    
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.GetProjectionMatrix();
    
    m_terrainShader->SetMat4("view", view);
    m_terrainShader->SetMat4("projection", projection);
    m_terrainShader->SetVec3("terrainColor", m_terrainColor);
    m_terrainShader->SetVec3("viewPos", cameraPos);
    m_terrainShader->SetVec3("lightDir", glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f)));
    
    // Render all visible chunks
    int centerChunkX = static_cast<int>(std::floor(cameraPos.x / m_chunkSize));
    int centerChunkZ = static_cast<int>(std::floor(cameraPos.z / m_chunkSize));
    
    for (int x = centerChunkX - m_renderDistance; x <= centerChunkX + m_renderDistance; x++) {
        for (int z = centerChunkZ - m_renderDistance; z <= centerChunkZ + m_renderDistance; z++) {
            int64_t key = GetChunkKey(x, z);
            auto it = m_chunks.find(key);
            
            if (it != m_chunks.end() && it->second.mesh) {
                glm::vec3 chunkWorldPos(
                    x * m_chunkSize,
                    0.0f,
                    z * m_chunkSize
                );
                
                glm::mat4 model = glm::translate(glm::mat4(1.0f), chunkWorldPos);
                m_terrainShader->SetMat4("model", model);
                
                it->second.mesh->Render();
            }
        }
    }
    
    m_terrainShader->Unbind();
}

void Terrain::UpdateChunks(const glm::vec3& cameraPos) {
    int centerChunkX = static_cast<int>(std::floor(cameraPos.x / m_chunkSize));
    int centerChunkZ = static_cast<int>(std::floor(cameraPos.z / m_chunkSize));
    
    glm::vec2 currentChunk(centerChunkX, centerChunkZ);
    
    // Only update if camera moved to a new chunk
    if (glm::distance(currentChunk, m_lastCameraChunk) < 0.5f) {
        return;
    }
    
    m_lastCameraChunk = currentChunk;
    
    // Generate chunks around camera
    for (int x = centerChunkX - m_renderDistance; x <= centerChunkX + m_renderDistance; x++) {
        for (int z = centerChunkZ - m_renderDistance; z <= centerChunkZ + m_renderDistance; z++) {
            int64_t key = GetChunkKey(x, z);
            
            if (m_chunks.find(key) == m_chunks.end()) {
                TerrainChunk chunk;
                chunk.position = glm::vec2(x, z);
                chunk.needsUpdate = true;
                GenerateChunk(chunk.position, chunk);
                m_chunks[key] = std::move(chunk);
            }
        }
    }
    
    // Remove far chunks to save memory
    auto it = m_chunks.begin();
    while (it != m_chunks.end()) {
        int chunkX = static_cast<int>(it->second.position.x);
        int chunkZ = static_cast<int>(it->second.position.y);
        
        int distX = abs(chunkX - centerChunkX);
        int distZ = abs(chunkZ - centerChunkZ);
        
        if (distX > m_renderDistance + 2 || distZ > m_renderDistance + 2) {
            it = m_chunks.erase(it);
        } else {
            ++it;
        }
    }
}

void Terrain::GenerateChunk(const glm::vec2& chunkPos, TerrainChunk& chunk) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    float step = static_cast<float>(m_chunkSize) / m_gridResolution;
    
    // Generate vertices
    for (int z = 0; z <= m_gridResolution; z++) {
        for (int x = 0; x <= m_gridResolution; x++) {
            float worldX = x * step;
            float worldZ = z * step;
            
            float absoluteX = chunkPos.x * m_chunkSize + worldX;
            float absoluteZ = chunkPos.y * m_chunkSize + worldZ;
            
            float height = GenerateHeight(absoluteX, absoluteZ);
            
            Vertex vertex;
            vertex.position = glm::vec3(worldX, height, worldZ);
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            vertex.texCoords = glm::vec2(
                static_cast<float>(x) / m_gridResolution,
                static_cast<float>(z) / m_gridResolution
            );
            
            vertices.push_back(vertex);
        }
    }
    
    // Calculate normals
    for (int z = 0; z < m_gridResolution; z++) {
        for (int x = 0; x < m_gridResolution; x++) {
            int i0 = z * (m_gridResolution + 1) + x;
            int i1 = i0 + 1;
            int i2 = i0 + (m_gridResolution + 1);
            int i3 = i2 + 1;
            
            glm::vec3 v0 = vertices[i0].position;
            glm::vec3 v1 = vertices[i1].position;
            glm::vec3 v2 = vertices[i2].position;
            glm::vec3 v3 = vertices[i3].position;
            
            glm::vec3 normal1 = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            glm::vec3 normal2 = glm::normalize(glm::cross(v3 - v1, v2 - v1));
            
            vertices[i0].normal += normal1;
            vertices[i1].normal += normal1 + normal2;
            vertices[i2].normal += normal1 + normal2;
            vertices[i3].normal += normal2;
        }
    }
    
    // Normalize all normals
    for (auto& vertex : vertices) {
        vertex.normal = glm::normalize(vertex.normal);
    }
    
    // Generate indices
    for (int z = 0; z < m_gridResolution; z++) {
        for (int x = 0; x < m_gridResolution; x++) {
            int topLeft = z * (m_gridResolution + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (m_gridResolution + 1) + x;
            int bottomRight = bottomLeft + 1;
            
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
    chunk.mesh = std::make_unique<Mesh>();
    chunk.mesh->SetVertices(vertices);
    chunk.mesh->SetIndices(indices);
    chunk.mesh->Upload();
}

float Terrain::GenerateHeight(float x, float z) const {
    // MSFS-style procedural terrain generation
    // Multiple octaves of noise for realistic terrain
    float height = 0.0f;
    
    // Large-scale features
    height += 50.0f * sin(x * 0.001f) * cos(z * 0.001f);
    
    // Medium-scale hills
    height += 20.0f * sin(x * 0.005f + z * 0.003f);
    
    // Small-scale variation
    height += 5.0f * sin(x * 0.02f) * sin(z * 0.02f);
    
    // Ensure minimum height
    height = std::max(0.0f, height);
    
    return height;
}

float Terrain::GetHeightAt(float x, float z) const {
    return GenerateHeight(x, z);
}

glm::vec3 Terrain::GetNormalAt(float x, float z) const {
    float epsilon = 1.0f;
    float h = GetHeightAt(x, z);
    float hx = GetHeightAt(x + epsilon, z);
    float hz = GetHeightAt(x, z + epsilon);
    
    glm::vec3 tangentX(epsilon, hx - h, 0.0f);
    glm::vec3 tangentZ(0.0f, hz - h, epsilon);
    
    return glm::normalize(glm::cross(tangentZ, tangentX));
}

int64_t Terrain::GetChunkKey(int x, int z) const {
    return (static_cast<int64_t>(x) << 32) | (static_cast<int64_t>(z) & 0xFFFFFFFF);
}

void Terrain::CreateTerrainMesh() {
    // Base mesh is created per-chunk in GenerateChunk
}

void Terrain::SetupShaders() {
    m_terrainShader = std::make_unique<Shader>();
    
    std::string vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoord;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoord;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoord = aTexCoord;
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
    
    std::string fragmentSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoord;
        
        uniform vec3 terrainColor;
        uniform vec3 viewPos;
        uniform vec3 lightDir;
        
        void main() {
            // MSFS-style terrain shading
            vec3 norm = normalize(Normal);
            
            // Diffuse lighting
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * terrainColor;
            
            // Ambient with sky color influence
            vec3 skyColor = vec3(0.4, 0.5, 0.7);
            vec3 ambient = 0.5 * mix(terrainColor, skyColor, 0.3);
            
            // Height-based color variation
            float heightFactor = clamp(FragPos.y / 100.0, 0.0, 1.0);
            vec3 snowColor = vec3(0.9, 0.9, 0.95);
            vec3 baseColor = mix(terrainColor, snowColor, heightFactor * 0.3);
            
            vec3 result = (ambient + diffuse) * baseColor;
            
            // Atmospheric fog - MSFS style
            float dist = length(viewPos - FragPos);
            float fogStart = 2000.0;
            float fogEnd = 20000.0;
            float fogFactor = clamp((fogEnd - dist) / (fogEnd - fogStart), 0.0, 1.0);
            
            vec3 fogColor = vec3(0.7, 0.8, 0.9);
            result = mix(fogColor, result, fogFactor);
            
            // Grid lines for depth perception (subtle)
            float gridSize = 200.0;
            vec2 grid = abs(fract(FragPos.xz / gridSize - 0.5) - 0.5) / fwidth(FragPos.xz / gridSize);
            float gridLine = min(grid.x, grid.y);
            float gridStrength = 1.0 - min(gridLine, 1.0);
            
            // Fade grid with distance
            float gridFade = smoothstep(3000.0, 8000.0, dist);
            gridStrength *= (1.0 - gridFade) * 0.15;
            
            vec3 gridColor = vec3(0.3, 0.4, 0.5);
            result = mix(result, gridColor, gridStrength);
            
            FragColor = vec4(result, 1.0);
        }
    )";
    
    m_terrainShader->LoadFromStrings(vertexSource, fragmentSource);
}

} // namespace FlightSim

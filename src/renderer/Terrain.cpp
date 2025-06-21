#include "renderer/Terrain.h"
#include "core/Camera.h"
#include <glad/glad.h>
#include <iostream>

namespace FlightSim {

Terrain::Terrain()
    : m_gridSize(100)
    , m_terrainScale(1000.0f)
    , m_terrainColor(0.3f, 0.7f, 0.2f)
    , m_terrainWidth(100)
    , m_terrainHeight(100) {
}

Terrain::~Terrain() {
    Shutdown();
}

bool Terrain::Initialize() {
    SetupShaders();
    GenerateTerrain(m_terrainWidth, m_terrainHeight, m_terrainScale);
    CreateTerrainMesh();
    return true;
}

void Terrain::Shutdown() {
    m_terrainMesh.reset();
    m_terrainShader.reset();
}

void Terrain::Render(const Camera& camera) {
    if (!m_terrainShader || !m_terrainMesh) return;
    
    m_terrainShader->Use();
    
    // Set matrices
    glm::mat4 model = glm::mat4(1.0f); // Identity matrix for terrain
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.GetProjectionMatrix();
    
    m_terrainShader->SetMat4("model", model);
    m_terrainShader->SetMat4("view", view);
    m_terrainShader->SetMat4("projection", projection);
    
    // Set terrain color
    m_terrainShader->SetVec3("terrainColor", m_terrainColor);
    m_terrainShader->SetVec3("viewPos", camera.GetPosition());
    
    // Render terrain mesh
    m_terrainMesh->Render();
    
    m_terrainShader->Unbind();
}

void Terrain::GenerateTerrain(int width, int height, float scale) {
    m_terrainWidth = width;
    m_terrainHeight = height;
    m_terrainScale = scale;
    
    // Initialize height map
    m_heightMap.resize(height);
    for (int i = 0; i < height; ++i) {
        m_heightMap[i].resize(width, 0.0f);
    }
    
    // Generate simple flat terrain for now
    // Could be extended with noise generation for realistic terrain
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            m_heightMap[i][j] = 0.0f; // Flat terrain
        }
    }
}

float Terrain::GetHeightAt(float x, float z) const {
    // Convert world coordinates to terrain grid coordinates
    float gridX = (x / m_terrainScale + 0.5f) * m_terrainWidth;
    float gridZ = (z / m_terrainScale + 0.5f) * m_terrainHeight;
    
    int ix = static_cast<int>(gridX);
    int iz = static_cast<int>(gridZ);
    
    // Clamp to valid range
    if (ix < 0 || ix >= m_terrainWidth - 1 || iz < 0 || iz >= m_terrainHeight - 1) {
        return 0.0f;
    }
    
    // Simple bilinear interpolation
    float fx = gridX - ix;
    float fz = gridZ - iz;
    
    float h00 = m_heightMap[iz][ix];
    float h10 = m_heightMap[iz][ix + 1];
    float h01 = m_heightMap[iz + 1][ix];
    float h11 = m_heightMap[iz + 1][ix + 1];
    
    float h0 = h00 * (1.0f - fx) + h10 * fx;
    float h1 = h01 * (1.0f - fx) + h11 * fx;
    
    return h0 * (1.0f - fz) + h1 * fz;
}

void Terrain::CreateTerrainMesh() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // Generate vertices
    for (int i = 0; i < m_terrainHeight; ++i) {
        for (int j = 0; j < m_terrainWidth; ++j) {
            float x = (float(j) / float(m_terrainWidth - 1) - 0.5f) * m_terrainScale;
            float z = (float(i) / float(m_terrainHeight - 1) - 0.5f) * m_terrainScale;
            float y = m_heightMap[i][j];
            
            float u = float(j) / float(m_terrainWidth - 1);
            float v = float(i) / float(m_terrainHeight - 1);
            
            Vertex vertex;
            vertex.position = glm::vec3(x, y, z);
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f); // Simple upward normal
            vertex.texCoords = glm::vec2(u, v);
            
            vertices.push_back(vertex);
        }
    }
    
    // Generate indices
    for (int i = 0; i < m_terrainHeight - 1; ++i) {
        for (int j = 0; j < m_terrainWidth - 1; ++j) {
            int topLeft = i * m_terrainWidth + j;
            int topRight = topLeft + 1;
            int bottomLeft = (i + 1) * m_terrainWidth + j;
            int bottomRight = bottomLeft + 1;
            
            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
    // Create mesh
    m_terrainMesh = std::make_unique<Mesh>();
    m_terrainMesh->SetVertices(vertices);
    m_terrainMesh->SetIndices(indices);
    m_terrainMesh->Upload();
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
        
        void main() {
            vec3 lightDir = normalize(vec3(-0.3, -1.0, -0.2));
            vec3 norm = normalize(Normal);
            
            float diff = max(dot(norm, -lightDir), 0.0);
            vec3 diffuse = diff * terrainColor;
            
            vec3 ambient = 0.3 * terrainColor;
            vec3 result = ambient + diffuse;
            
            FragColor = vec4(result, 1.0);
        }
    )";
    
    m_terrainShader->LoadFromStrings(vertexSource, fragmentSource);
}

} // namespace FlightSim 
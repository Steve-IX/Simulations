#include "core/Mesh.h"
#include <glad/glad.h>
#include <iostream>
#include <cmath>

namespace FlightSim {

Mesh::Mesh() : m_VAO(0), m_VBO(0), m_EBO(0), m_uploaded(false) {
}

Mesh::~Mesh() {
    Cleanup();
}

void Mesh::SetVertices(const std::vector<Vertex>& vertices) {
    m_vertices = vertices;
    m_uploaded = false;
}

void Mesh::SetIndices(const std::vector<unsigned int>& indices) {
    m_indices = indices;
    m_uploaded = false;
}

void Mesh::Upload() {
    if (m_vertices.empty()) return;
    
    // Generate buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    if (!m_indices.empty()) {
        glGenBuffers(1, &m_EBO);
    }
    
    glBindVertexArray(m_VAO);
    
    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);
    
    // Upload index data
    if (!m_indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
    }
    
    // Setup vertex attributes
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    
    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    m_uploaded = true;
}

void Mesh::Render() const {
    if (!m_uploaded) return;
    
    glBindVertexArray(m_VAO);
    
    if (!m_indices.empty()) {
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    }
    
    glBindVertexArray(0);
}

void Mesh::RenderInstanced(int instanceCount) const {
    if (!m_uploaded || instanceCount <= 0) return;
    
    glBindVertexArray(m_VAO);
    
    if (!m_indices.empty()) {
        glDrawElementsInstanced(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0, instanceCount);
    } else {
        glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertices.size(), instanceCount);
    }
    
    glBindVertexArray(0);
}

Mesh Mesh::CreateCube() {
    Mesh mesh;
    
    std::vector<Vertex> vertices = {
        // Front face
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        
        // Back face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
        
        // Left face
        {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        
        // Right face
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        
        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
        
        // Top face
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}
    };
    
    std::vector<unsigned int> indices = {
        0,  1,  2,   2,  3,  0,   // Front
        4,  5,  6,   6,  7,  4,   // Back
        8,  9,  10,  10, 11, 8,   // Left
        12, 13, 14,  14, 15, 12,  // Right
        16, 17, 18,  18, 19, 16,  // Bottom
        20, 21, 22,  22, 23, 20   // Top
    };
    
    mesh.SetVertices(vertices);
    mesh.SetIndices(indices);
    return mesh;
}

Mesh Mesh::CreateSphere(int segments) {
    Mesh mesh;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    const float PI = 3.14159265359f;
    
    // Generate vertices
    for (int i = 0; i <= segments; ++i) {
        float phi = PI * float(i) / float(segments);
        for (int j = 0; j <= segments; ++j) {
            float theta = 2.0f * PI * float(j) / float(segments);
            
            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);
            
            float u = float(j) / float(segments);
            float v = float(i) / float(segments);
            
            vertices.push_back({
                {x, y, z},
                {x, y, z}, // Normal is same as position for sphere
                {u, v}
            });
        }
    }
    
    // Generate indices
    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < segments; ++j) {
            int first = i * (segments + 1) + j;
            int second = first + segments + 1;
            
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);
            
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    
    mesh.SetVertices(vertices);
    mesh.SetIndices(indices);
    return mesh;
}

Mesh Mesh::CreatePlane(float width, float height) {
    Mesh mesh;
    
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    std::vector<Vertex> vertices = {
        {{-halfWidth, 0.0f, -halfHeight}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ halfWidth, 0.0f, -halfHeight}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ halfWidth, 0.0f,  halfHeight}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-halfWidth, 0.0f,  halfHeight}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
    };
    
    std::vector<unsigned int> indices = {
        0, 1, 2,  2, 3, 0
    };
    
    mesh.SetVertices(vertices);
    mesh.SetIndices(indices);
    return mesh;
}

Mesh Mesh::CreateAircraft() {
    Mesh mesh;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // Enhanced aircraft shape - fuselage, wings, tail, cockpit
    // Fuselage (tapered body for more realistic look)
    float fuselageLength = 12.0f;
    float fuselageWidth = 1.2f;
    float fuselageHeight = 1.2f;
    
    // Nose section (more vertices for smoother shape)
    vertices.push_back({{ fuselageLength*0.5f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.5f}});  // Nose tip
    
    // Fuselage cross-sections
    const float PI = 3.14159265359f;
    for (int i = 0; i < 8; ++i) {
        float angle = i * PI / 4.0f;
        float x = fuselageLength * 0.3f;
        float y = sin(angle) * fuselageHeight * 0.5f;
        float z = cos(angle) * fuselageWidth * 0.5f;
        vertices.push_back({{x, y, z}, glm::normalize(glm::vec3(0.2f, y, z)), {float(i)/8.0f, 0.3f}});
    }
    
    // Mid section (largest diameter)
    for (int i = 0; i < 8; ++i) {
        float angle = i * PI / 4.0f;
        float x = 0.0f;
        float y = sin(angle) * fuselageHeight * 0.6f;
        float z = cos(angle) * fuselageWidth * 0.6f;
        vertices.push_back({{x, y, z}, glm::normalize(glm::vec3(0.0f, y, z)), {float(i)/8.0f, 0.5f}});
    }
    
    // Tail section
    for (int i = 0; i < 8; ++i) {
        float angle = i * PI / 4.0f;
        float x = -fuselageLength * 0.4f;
        float y = sin(angle) * fuselageHeight * 0.3f;
        float z = cos(angle) * fuselageWidth * 0.3f;
        vertices.push_back({{x, y, z}, glm::normalize(glm::vec3(-0.2f, y, z)), {float(i)/8.0f, 0.7f}});
    }
    
    // Tail end
    vertices.push_back({{-fuselageLength*0.5f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.5f, 1.0f}});
    
    int baseIdx = vertices.size();
    
    // Wings (swept back for more dynamic look)
    float wingSpan = 10.0f;
    float wingChord = 2.5f;
    float wingThickness = 0.15f;
    float wingSweep = 0.3f;  // Sweep angle
    
    // Main wings with dihedral angle
    float dihedralAngle = 0.05f;
    
    // Left wing
    vertices.push_back({{-wingChord*0.3f + wingSweep, -wingThickness, -fuselageWidth*0.5f}, 
                       {0.0f, -0.8f, -0.2f}, {0.0f, 0.0f}});
    vertices.push_back({{ wingChord*0.7f + wingSweep, -wingThickness, -fuselageWidth*0.5f}, 
                       {0.0f, -0.8f, -0.2f}, {1.0f, 0.0f}});
    vertices.push_back({{ wingChord*0.3f + wingSweep*2, -wingThickness + dihedralAngle, -wingSpan*0.5f}, 
                       {0.0f, -0.7f, -0.3f}, {0.8f, 1.0f}});
    vertices.push_back({{-wingChord*0.1f + wingSweep*2, -wingThickness + dihedralAngle, -wingSpan*0.5f}, 
                       {0.0f, -0.7f, -0.3f}, {0.2f, 1.0f}});
    
    // Right wing
    vertices.push_back({{-wingChord*0.3f + wingSweep, -wingThickness,  fuselageWidth*0.5f}, 
                       {0.0f, -0.8f, 0.2f}, {0.0f, 0.0f}});
    vertices.push_back({{ wingChord*0.7f + wingSweep, -wingThickness,  fuselageWidth*0.5f}, 
                       {0.0f, -0.8f, 0.2f}, {1.0f, 0.0f}});
    vertices.push_back({{ wingChord*0.3f + wingSweep*2, -wingThickness + dihedralAngle,  wingSpan*0.5f}, 
                       {0.0f, -0.7f, 0.3f}, {0.8f, 1.0f}});
    vertices.push_back({{-wingChord*0.1f + wingSweep*2, -wingThickness + dihedralAngle,  wingSpan*0.5f}, 
                       {0.0f, -0.7f, 0.3f}, {0.2f, 1.0f}});
    
    // Tail surfaces (horizontal stabilizer)
    float tailSpan = 4.0f;
    float tailChord = 1.5f;
    float tailPos = -fuselageLength * 0.35f;
    
    vertices.push_back({{tailPos - tailChord*0.5f, 0.3f, -tailSpan*0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{tailPos + tailChord*0.5f, 0.3f, -tailSpan*0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{tailPos + tailChord*0.5f, 0.3f,  tailSpan*0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{tailPos - tailChord*0.5f, 0.3f,  tailSpan*0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}});
    
    // Vertical stabilizer
    vertices.push_back({{tailPos - tailChord*0.3f, 0.3f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{tailPos + tailChord*0.3f, 0.3f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{tailPos + tailChord*0.1f, 2.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.8f, 1.0f}});
    vertices.push_back({{tailPos - tailChord*0.1f, 2.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.2f, 1.0f}});
    
    // Fuselage indices (connecting nose to sections)
    for (int i = 0; i < 8; ++i) {
        int next = (i + 1) % 8;
        // Nose to first ring
        indices.push_back(0);
        indices.push_back(1 + i);
        indices.push_back(1 + next);
        
        // First ring to mid section
        indices.push_back(1 + i);
        indices.push_back(9 + i);
        indices.push_back(9 + next);
        indices.push_back(9 + next);
        indices.push_back(1 + next);
        indices.push_back(1 + i);
        
        // Mid section to tail section
        indices.push_back(9 + i);
        indices.push_back(17 + i);
        indices.push_back(17 + next);
        indices.push_back(17 + next);
        indices.push_back(9 + next);
        indices.push_back(9 + i);
        
        // Tail section to tail end
        indices.push_back(17 + i);
        indices.push_back(25);
        indices.push_back(17 + next);
    }
    
    // Wing indices (left and right)
    int wingBase = baseIdx;
    indices.push_back(wingBase + 0);
    indices.push_back(wingBase + 1);
    indices.push_back(wingBase + 2);
    indices.push_back(wingBase + 2);
    indices.push_back(wingBase + 3);
    indices.push_back(wingBase + 0);
    
    indices.push_back(wingBase + 4);
    indices.push_back(wingBase + 5);
    indices.push_back(wingBase + 6);
    indices.push_back(wingBase + 6);
    indices.push_back(wingBase + 7);
    indices.push_back(wingBase + 4);
    
    // Horizontal stabilizer
    int tailBase = wingBase + 8;
    indices.push_back(tailBase + 0);
    indices.push_back(tailBase + 1);
    indices.push_back(tailBase + 2);
    indices.push_back(tailBase + 2);
    indices.push_back(tailBase + 3);
    indices.push_back(tailBase + 0);
    
    // Vertical stabilizer
    int vertBase = tailBase + 4;
    indices.push_back(vertBase + 0);
    indices.push_back(vertBase + 1);
    indices.push_back(vertBase + 2);
    indices.push_back(vertBase + 2);
    indices.push_back(vertBase + 3);
    indices.push_back(vertBase + 0);
    
    mesh.SetVertices(vertices);
    mesh.SetIndices(indices);
    return mesh;
}

void Mesh::SetupMesh() {
    // This is handled in Upload()
}

void Mesh::Cleanup() {
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    if (m_EBO != 0) {
        glDeleteBuffers(1, &m_EBO);
        m_EBO = 0;
    }
    m_uploaded = false;
}

} // namespace FlightSim 
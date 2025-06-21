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
    
    // Simple aircraft shape - fuselage, wings, tail
    // Fuselage (elongated box)
    float fuselageLength = 10.0f;
    float fuselageWidth = 1.0f;
    float fuselageHeight = 1.0f;
    
    // Fuselage vertices
    vertices.push_back({{-fuselageLength*0.5f, -fuselageHeight*0.5f, -fuselageWidth*0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ fuselageLength*0.5f, -fuselageHeight*0.5f, -fuselageWidth*0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ fuselageLength*0.5f, -fuselageHeight*0.5f,  fuselageWidth*0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-fuselageLength*0.5f, -fuselageHeight*0.5f,  fuselageWidth*0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}});
    
    vertices.push_back({{-fuselageLength*0.5f,  fuselageHeight*0.5f, -fuselageWidth*0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ fuselageLength*0.5f,  fuselageHeight*0.5f, -fuselageWidth*0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ fuselageLength*0.5f,  fuselageHeight*0.5f,  fuselageWidth*0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-fuselageLength*0.5f,  fuselageHeight*0.5f,  fuselageWidth*0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}});
    
    // Wings (simple rectangular wings)
    float wingSpan = 8.0f;
    float wingChord = 2.0f;
    float wingThickness = 0.2f;
    
    // Left wing
    vertices.push_back({{-wingChord*0.5f, -wingThickness*0.5f, -wingSpan*0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ wingChord*0.5f, -wingThickness*0.5f, -wingSpan*0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ wingChord*0.5f, -wingThickness*0.5f, -fuselageWidth*0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.5f}});
    vertices.push_back({{-wingChord*0.5f, -wingThickness*0.5f, -fuselageWidth*0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.5f}});
    
    // Right wing
    vertices.push_back({{-wingChord*0.5f, -wingThickness*0.5f,  fuselageWidth*0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.5f}});
    vertices.push_back({{ wingChord*0.5f, -wingThickness*0.5f,  fuselageWidth*0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.5f}});
    vertices.push_back({{ wingChord*0.5f, -wingThickness*0.5f,  wingSpan*0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-wingChord*0.5f, -wingThickness*0.5f,  wingSpan*0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}});
    
    // Fuselage indices
    std::vector<unsigned int> fuselageIndices = {
        0, 1, 2,  2, 3, 0,  // Bottom
        4, 7, 6,  6, 5, 4,  // Top
        0, 4, 5,  5, 1, 0,  // Front
        2, 6, 7,  7, 3, 2,  // Back
        0, 3, 7,  7, 4, 0,  // Left
        1, 5, 6,  6, 2, 1   // Right
    };
    
    // Wing indices
    std::vector<unsigned int> wingIndices = {
        8,  9,  10, 10, 11, 8,   // Left wing
        12, 13, 14, 14, 15, 12   // Right wing
    };
    
    // Combine indices
    for (auto idx : fuselageIndices) indices.push_back(idx);
    for (auto idx : wingIndices) indices.push_back(idx);
    
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
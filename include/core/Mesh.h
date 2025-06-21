#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace FlightSim {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh {
public:
    Mesh();
    ~Mesh();
    
    void SetVertices(const std::vector<Vertex>& vertices);
    void SetIndices(const std::vector<unsigned int>& indices);
    void Upload();
    
    void Render() const;
    void RenderInstanced(int instanceCount) const;
    
    // Utility functions for creating basic shapes
    static Mesh CreateCube();
    static Mesh CreateSphere(int segments = 32);
    static Mesh CreatePlane(float width = 1.0f, float height = 1.0f);
    static Mesh CreateAircraft(); // Simple aircraft mesh
    
    bool IsUploaded() const { return m_uploaded; }
    
private:
    void SetupMesh();
    void Cleanup();
    
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    
    unsigned int m_VAO, m_VBO, m_EBO;
    bool m_uploaded;
};

} // namespace FlightSim 
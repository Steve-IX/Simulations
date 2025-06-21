#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace FlightSim {

class Shader {
public:
    Shader();
    ~Shader();
    
    bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    bool LoadFromStrings(const std::string& vertexSource, const std::string& fragmentSource);
    
    void Use() const;
    void Unbind() const;
    
    // Uniform setters
    void SetBool(const std::string& name, bool value);
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec2(const std::string& name, const glm::vec2& value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetMat3(const std::string& name, const glm::mat3& value);
    void SetMat4(const std::string& name, const glm::mat4& value);
    
    unsigned int GetID() const { return m_programID; }
    bool IsValid() const { return m_programID != 0; }
    
private:
    unsigned int CompileShader(const std::string& source, unsigned int type);
    bool LinkProgram(unsigned int vertexShader, unsigned int fragmentShader);
    int GetUniformLocation(const std::string& name);
    std::string ReadFile(const std::string& filePath);
    
    unsigned int m_programID;
    mutable std::unordered_map<std::string, int> m_uniformLocationCache;
};

} // namespace FlightSim 
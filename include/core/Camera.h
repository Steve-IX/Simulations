#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace FlightSim {

class Aircraft;

enum class CameraMode {
    Cockpit,
    External,
    Chase,
    Free
};

class Camera {
public:
    Camera(float fov = 45.0f, float aspect = 16.0f/9.0f, float nearPlane = 0.1f, float farPlane = 10000.0f);
    
    void Update(const glm::vec3& aircraftPosition, const glm::vec3& aircraftForward, 
                const glm::vec3& aircraftUp, float deltaTime);
    void Update(const Aircraft& aircraft, float deltaTime);
    
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::vec3 GetPosition() const { return m_position; }
    glm::vec3 GetFront() const { return m_front; }
    glm::vec3 GetUp() const { return m_up; }
    glm::vec3 GetRight() const { return m_right; }
    
    void SetAspectRatio(float aspect);
    void SetMode(CameraMode mode) { m_mode = mode; }
    CameraMode GetMode() const { return m_mode; }
    void CycleMode();
    
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    void ProcessKeyboard(int key, float deltaTime);
    
private:
    void UpdateCameraVectors();
    void UpdateCockpitCamera(const glm::vec3& aircraftPos, const glm::vec3& aircraftForward, const glm::vec3& aircraftUp);
    void UpdateExternalCamera(const glm::vec3& aircraftPos, const glm::vec3& aircraftForward, const glm::vec3& aircraftUp);
    void UpdateChaseCamera(const glm::vec3& aircraftPos, const glm::vec3& aircraftForward, const glm::vec3& aircraftUp);
    void UpdateFreeCamera(float deltaTime);
    
    // Camera attributes
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;
    
    // Camera options
    float m_fov;
    float m_aspect;
    float m_nearPlane;
    float m_farPlane;
    
    // Euler angles
    float m_yaw;
    float m_pitch;
    
    // Camera mode
    CameraMode m_mode;
    
    // Mouse sensitivity
    float m_mouseSensitivity;
    float m_movementSpeed;
    
    // External camera settings
    float m_externalDistance;
    float m_externalHeight;
    
    // Free camera movement
    bool m_keys[1024];
};

} // namespace FlightSim 
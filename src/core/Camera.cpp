#include "core/Camera.h"
#include "physics/Aircraft.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstring>

namespace FlightSim {

Camera::Camera(float fov, float aspect, float nearPlane, float farPlane)
    : m_position(0.0f, 0.0f, 3.0f)
    , m_front(0.0f, 0.0f, -1.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_right(1.0f, 0.0f, 0.0f)
    , m_worldUp(0.0f, 1.0f, 0.0f)
    , m_fov(fov)
    , m_aspect(aspect)
    , m_nearPlane(nearPlane)
    , m_farPlane(farPlane)
    , m_yaw(-90.0f)
    , m_pitch(0.0f)
    , m_mode(CameraMode::External)
    , m_mouseSensitivity(0.1f)
    , m_movementSpeed(10.0f)
    , m_externalDistance(20.0f)
    , m_externalHeight(5.0f) {
    
    // Initialize keys array
    for (int i = 0; i < 1024; ++i) {
        m_keys[i] = false;
    }
    
    UpdateCameraVectors();
}

void Camera::Update(const Aircraft& aircraft, float deltaTime) {
    const AircraftState& state = aircraft.GetState();
    glm::vec3 aircraftForward = aircraft.GetForward();
    glm::vec3 aircraftUp = aircraft.GetUp();
    
    Update(state.position, aircraftForward, aircraftUp, deltaTime);
}

void Camera::Update(const glm::vec3& aircraftPosition, const glm::vec3& aircraftForward, 
                   const glm::vec3& aircraftUp, float deltaTime) {
    switch (m_mode) {
        case CameraMode::Cockpit:
            UpdateCockpitCamera(aircraftPosition, aircraftForward, aircraftUp);
            break;
        case CameraMode::External:
            UpdateExternalCamera(aircraftPosition, aircraftForward, aircraftUp);
            break;
        case CameraMode::Chase:
            UpdateChaseCamera(aircraftPosition, aircraftForward, aircraftUp);
            break;
        case CameraMode::Free:
            UpdateFreeCamera(deltaTime);
            break;
    }
}

void Camera::CycleMode() {
    switch (m_mode) {
        case CameraMode::Cockpit:
            m_mode = CameraMode::External;
            break;
        case CameraMode::External:
            m_mode = CameraMode::Chase;
            break;
        case CameraMode::Chase:
            m_mode = CameraMode::Free;
            break;
        case CameraMode::Free:
            m_mode = CameraMode::Cockpit;
            break;
    }
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspect, m_nearPlane, m_farPlane);
}

void Camera::SetAspectRatio(float aspect) {
    m_aspect = aspect;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;
    
    m_yaw += xoffset;
    m_pitch += yoffset;
    
    if (constrainPitch) {
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    }
    
    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    m_fov -= yoffset;
    m_fov = std::clamp(m_fov, 1.0f, 120.0f);
}

void Camera::ProcessKeyboard(int key, float deltaTime) {
    if (key >= 0 && key < 1024) {
        if (glfwGetKey(glfwGetCurrentContext(), key) == GLFW_PRESS) {
            m_keys[key] = true;
        } else {
            m_keys[key] = false;
        }
    }
}

void Camera::UpdateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    
    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::UpdateCockpitCamera(const glm::vec3& aircraftPos, const glm::vec3& aircraftForward, const glm::vec3& aircraftUp) {
    // Position camera slightly forward and up from aircraft center
    m_position = aircraftPos + aircraftForward * 2.0f + aircraftUp * 1.0f;
    m_front = aircraftForward;
    m_up = aircraftUp;
    m_right = glm::normalize(glm::cross(m_front, m_up));
}

void Camera::UpdateExternalCamera(const glm::vec3& aircraftPos, const glm::vec3& aircraftForward, const glm::vec3& aircraftUp) {
    // Position camera behind and above the aircraft
    glm::vec3 offset = -aircraftForward * m_externalDistance + aircraftUp * m_externalHeight;
    m_position = aircraftPos + offset;
    
    // Look at the aircraft
    m_front = glm::normalize(aircraftPos - m_position);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::UpdateChaseCamera(const glm::vec3& aircraftPos, const glm::vec3& aircraftForward, const glm::vec3& aircraftUp) {
    // Similar to external but closer and following more smoothly
    float chaseDistance = 20.0f;
    float chaseHeight = 5.0f;
    
    glm::vec3 targetPos = aircraftPos - aircraftForward * chaseDistance + aircraftUp * chaseHeight;
    
    // Smooth interpolation
    float lerpFactor = 0.05f;
    m_position = glm::mix(m_position, targetPos, lerpFactor);
    
    // Look at aircraft
    m_front = glm::normalize(aircraftPos - m_position);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::UpdateFreeCamera(float deltaTime) {
    float velocity = m_movementSpeed * deltaTime;
    
    if (m_keys[GLFW_KEY_W])
        m_position += m_front * velocity;
    if (m_keys[GLFW_KEY_S])
        m_position -= m_front * velocity;
    if (m_keys[GLFW_KEY_A])
        m_position -= m_right * velocity;
    if (m_keys[GLFW_KEY_D])
        m_position += m_right * velocity;
    if (m_keys[GLFW_KEY_SPACE])
        m_position += m_worldUp * velocity;
    if (m_keys[GLFW_KEY_LEFT_CONTROL])
        m_position -= m_worldUp * velocity;
    
    UpdateCameraVectors();
}

} // namespace FlightSim 
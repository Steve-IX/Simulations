#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include "FlightDynamics.h"

namespace FlightSim {

struct ControlInputs {
    float aileron = 0.0f;    // -1.0 to 1.0 (roll)
    float elevator = 0.0f;   // -1.0 to 1.0 (pitch)
    float rudder = 0.0f;     // -1.0 to 1.0 (yaw)
    float throttle = 0.0f;   // 0.0 to 1.0
    float flaps = 0.0f;      // 0.0 to 1.0
    float brakes = 0.0f;     // 0.0 to 1.0
};

struct AircraftState {
    glm::vec3 position{0.0f, 1000.0f, 0.0f};  // World position (meters)
    glm::vec3 velocity{0.0f, 0.0f, 0.0f};     // Linear velocity (m/s)
    glm::quat orientation{1.0f, 0.0f, 0.0f, 0.0f}; // Orientation quaternion
    glm::vec3 angularVelocity{0.0f, 0.0f, 0.0f};   // Angular velocity (rad/s)
    
    // Derived values
    float airspeed = 0.0f;        // m/s
    float altitude = 1000.0f;     // meters above sea level
    float verticalSpeed = 0.0f;   // m/s (positive = climbing)
    float heading = 0.0f;         // degrees
    float pitch = 0.0f;           // degrees
    float roll = 0.0f;            // degrees
};

class Aircraft {
public:
    Aircraft();
    ~Aircraft();
    
    void Initialize();
    void Update(float deltaTime, const ControlInputs& controls);
    
    const AircraftState& GetState() const { return m_state; }
    glm::mat4 GetModelMatrix() const;
    
    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;
    
    // Flight parameters
    void SetPosition(const glm::vec3& position);
    void SetOrientation(const glm::quat& orientation);
    void Reset(); // Reset to initial state
    
    // Configuration
    void SetAircraftType(const std::string& type);
    
private:
    void UpdateDerivedValues();
    void IntegratePhysics(float deltaTime, const glm::vec3& forces, const glm::vec3& torques);
    
    AircraftState m_state;
    FlightDynamics m_dynamics;
    
    // Aircraft specifications
    float m_mass;           // kg
    glm::mat3 m_inertiaTensor; // kg*m^2
    
    // Engine parameters
    float m_maxThrust;      // Newtons
    float m_currentThrust;  // Newtons
    
    // Control surface effectiveness
    float m_aileronEffectiveness;
    float m_elevatorEffectiveness;
    float m_rudderEffectiveness;
};

} // namespace FlightSim 
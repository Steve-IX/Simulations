#include "physics/Aircraft.h"
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>

namespace FlightSim {

Aircraft::Aircraft()
    : m_mass(1500.0f)  // kg (small aircraft)
    , m_maxThrust(8000.0f)  // Newtons
    , m_currentThrust(0.0f)
    , m_aileronEffectiveness(1.0f)
    , m_elevatorEffectiveness(1.0f)
    , m_rudderEffectiveness(1.0f) {
    
    // Initialize inertia tensor for typical small aircraft
    m_inertiaTensor = glm::mat3(
        2000.0f, 0.0f, 0.0f,
        0.0f, 3000.0f, 0.0f,
        0.0f, 0.0f, 4000.0f
    );
}

Aircraft::~Aircraft() {
}

void Aircraft::Initialize() {
    // Set up flight dynamics
    float wingArea = 16.0f;  // m²
    float wingspan = 10.0f;  // m
    
    m_dynamics.SetAircraftParameters(m_mass, wingArea, wingspan, m_inertiaTensor);
    
    // Reset to initial state
    Reset();
}

void Aircraft::Update(float deltaTime, const ControlInputs& controls) {
    // Calculate forces and torques
    glm::vec3 forces, torques;
    m_dynamics.CalculateForces(m_state, controls, forces, torques);
    
    // Add thrust
    glm::vec3 thrustForce = m_dynamics.CalculateThrustForce(m_state, controls.throttle, m_maxThrust);
    forces += thrustForce;
    
    // Integrate physics
    IntegratePhysics(deltaTime, forces, torques);
    
    // Update derived values
    UpdateDerivedValues();
}

glm::mat4 Aircraft::GetModelMatrix() const {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_state.position);
    glm::mat4 rotation = glm::mat4_cast(m_state.orientation);
    return translation * rotation;
}

glm::vec3 Aircraft::GetForward() const {
    return m_state.orientation * glm::vec3(0.0f, 0.0f, 1.0f);
}

glm::vec3 Aircraft::GetRight() const {
    return m_state.orientation * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 Aircraft::GetUp() const {
    return m_state.orientation * glm::vec3(0.0f, 1.0f, 0.0f);
}

void Aircraft::SetPosition(const glm::vec3& position) {
    m_state.position = position;
    UpdateDerivedValues();
}

void Aircraft::SetOrientation(const glm::quat& orientation) {
    m_state.orientation = orientation;
    UpdateDerivedValues();
}

void Aircraft::Reset() {
    // Reset to initial state
    m_state.position = glm::vec3(0.0f, 1000.0f, 0.0f);
    m_state.velocity = glm::vec3(0.0f, 0.0f, 30.0f);  // Start with forward velocity
    m_state.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    m_state.angularVelocity = glm::vec3(0.0f);
    
    UpdateDerivedValues();
}

void Aircraft::SetAircraftType(const std::string& type) {
    // Could be used to load different aircraft configurations
    // For now, just using default values
}

void Aircraft::UpdateDerivedValues() {
    // Calculate airspeed
    m_state.airspeed = glm::length(m_state.velocity);
    
    // Calculate altitude
    m_state.altitude = m_state.position.y;
    
    // Calculate vertical speed
    m_state.verticalSpeed = m_state.velocity.y;
    
    // Calculate Euler angles from quaternion
    glm::vec3 eulerAngles = glm::eulerAngles(m_state.orientation);
    m_state.pitch = glm::degrees(eulerAngles.x);
    m_state.roll = glm::degrees(eulerAngles.z);
    m_state.heading = glm::degrees(eulerAngles.y);
    
    // Normalize heading to 0-360 degrees
    while (m_state.heading < 0.0f) m_state.heading += 360.0f;
    while (m_state.heading >= 360.0f) m_state.heading -= 360.0f;
}

void Aircraft::IntegratePhysics(float deltaTime, const glm::vec3& forces, const glm::vec3& torques) {
    // Linear motion integration (Euler method for simplicity)
    glm::vec3 acceleration = forces / m_mass;
    m_state.velocity += acceleration * deltaTime;
    m_state.position += m_state.velocity * deltaTime;
    
    // Angular motion integration
    glm::mat3 invInertia = glm::inverse(m_inertiaTensor);
    glm::vec3 angularAcceleration = invInertia * torques;
    m_state.angularVelocity += angularAcceleration * deltaTime;
    
    // Update orientation using angular velocity
    glm::quat angularVelQuat(0.0f, m_state.angularVelocity.x, m_state.angularVelocity.y, m_state.angularVelocity.z);
    glm::quat orientationDerivative = 0.5f * angularVelQuat * m_state.orientation;
    m_state.orientation += orientationDerivative * deltaTime;
    m_state.orientation = glm::normalize(m_state.orientation);
    
    // Apply damping to prevent instability
    m_state.angularVelocity *= 0.99f;
    
    // Prevent aircraft from going underground
    if (m_state.position.y < 0.0f) {
        m_state.position.y = 0.0f;
        m_state.velocity.y = std::max(0.0f, m_state.velocity.y);
    }
}

} // namespace FlightSim 
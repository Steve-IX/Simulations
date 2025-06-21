#include "physics/FlightDynamics.h"
#include "physics/Aircraft.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <cmath>

namespace FlightSim {

FlightDynamics::FlightDynamics()
    : m_mass(1500.0f)
    , m_wingArea(16.0f)
    , m_wingspan(10.0f) {
}

void FlightDynamics::SetAircraftParameters(float mass, float wingArea, float wingspan, const glm::mat3& inertiaTensor) {
    m_mass = mass;
    m_wingArea = wingArea;
    m_wingspan = wingspan;
    m_inertiaTensor = inertiaTensor;
}

void FlightDynamics::SetAerodynamicCoefficients(const AerodynamicCoefficients& coeffs) {
    m_aeroCoeffs = coeffs;
}

void FlightDynamics::CalculateForces(const AircraftState& state, const ControlInputs& controls, 
                                   glm::vec3& forces, glm::vec3& torques) {
    // Calculate individual force components
    glm::vec3 aeroForces = CalculateAerodynamicForces(state, controls);
    glm::vec3 gravityForce = CalculateGravityForce(state);
    
    // Sum all forces
    forces = aeroForces + gravityForce;
    
    // Calculate torques
    torques = CalculateAerodynamicTorques(state, controls);
}

glm::vec3 FlightDynamics::CalculateAerodynamicForces(const AircraftState& state, const ControlInputs& controls) {
    float dynamicPressure = GetDynamicPressure(state);
    float angleOfAttack = GetAngleOfAttack(state);
    float sideslipAngle = GetSideslipAngle(state);
    
    // Calculate lift coefficient
    float liftCoeff = CalculateLiftCoefficient(angleOfAttack, controls.elevator);
    
    // Calculate drag coefficient
    float dragCoeff = CalculateDragCoefficient(liftCoeff);
    
    // Calculate side force coefficient
    float sideForceCoeff = m_aeroCoeffs.Cydr * controls.rudder;
    
    // Calculate forces in body frame
    float lift = liftCoeff * dynamicPressure * m_wingArea;
    float drag = dragCoeff * dynamicPressure * m_wingArea;
    float sideForce = sideForceCoeff * dynamicPressure * m_wingArea;
    
    // Forces in body coordinates (aircraft frame)
    glm::vec3 bodyForces(-drag, sideForce, -lift);  // X: forward, Y: right, Z: up
    
    // Transform to world coordinates
    return BodyToWorld(bodyForces, state.orientation);
}

glm::vec3 FlightDynamics::CalculateAerodynamicTorques(const AircraftState& state, const ControlInputs& controls) {
    float dynamicPressure = GetDynamicPressure(state);
    float angleOfAttack = GetAngleOfAttack(state);
    
    // Pitching moment
    float pitchingMoment = (m_aeroCoeffs.Cm0 + m_aeroCoeffs.Cma * angleOfAttack + m_aeroCoeffs.Cmde * controls.elevator)
                          * dynamicPressure * m_wingArea * (m_wingspan * 0.25f);  // Mean aerodynamic chord approximation
    
    // Rolling moment
    float rollingMoment = m_aeroCoeffs.Clda * controls.aileron * dynamicPressure * m_wingArea * m_wingspan;
    
    // Yawing moment
    float yawingMoment = m_aeroCoeffs.Cndr * controls.rudder * dynamicPressure * m_wingArea * m_wingspan;
    
    // Torques in body frame (roll, pitch, yaw)
    return glm::vec3(rollingMoment, pitchingMoment, yawingMoment);
}

glm::vec3 FlightDynamics::CalculateGravityForce(const AircraftState& state) {
    return glm::vec3(0.0f, -m_mass * 9.81f, 0.0f);  // Gravity in world frame
}

glm::vec3 FlightDynamics::CalculateThrustForce(const AircraftState& state, float throttle, float maxThrust) {
    float thrust = throttle * maxThrust;
    
    // Thrust acts along the aircraft's forward direction
    glm::vec3 forwardDir = state.orientation * glm::vec3(0.0f, 0.0f, 1.0f);
    return forwardDir * thrust;
}

float FlightDynamics::GetAirDensity(float altitude) const {
    // Simplified atmospheric model
    const float seaLevelDensity = 1.225f;  // kg/m³
    const float scaleHeight = 8400.0f;     // m
    
    return seaLevelDensity * exp(-altitude / scaleHeight);
}

float FlightDynamics::GetAngleOfAttack(const AircraftState& state) const {
    // Convert velocity to body frame
    glm::vec3 bodyVelocity = WorldToBody(state.velocity, state.orientation);
    
    // Angle of attack is the angle between velocity and body X-axis
    if (bodyVelocity.z != 0.0f) {
        return atan2(-bodyVelocity.y, bodyVelocity.z);
    }
    return 0.0f;
}

float FlightDynamics::GetSideslipAngle(const AircraftState& state) const {
    // Convert velocity to body frame
    glm::vec3 bodyVelocity = WorldToBody(state.velocity, state.orientation);
    
    // Sideslip angle is the angle between velocity projection and body X-axis
    float velocityMagnitudeXZ = sqrt(bodyVelocity.x * bodyVelocity.x + bodyVelocity.z * bodyVelocity.z);
    if (velocityMagnitudeXZ != 0.0f) {
        return atan2(bodyVelocity.x, velocityMagnitudeXZ);
    }
    return 0.0f;
}

float FlightDynamics::GetDynamicPressure(const AircraftState& state) const {
    float airDensity = GetAirDensity(state.altitude);
    float velocity = glm::length(state.velocity);
    return 0.5f * airDensity * velocity * velocity;
}

glm::vec3 FlightDynamics::WorldToBody(const glm::vec3& worldVector, const glm::quat& orientation) const {
    return glm::inverse(orientation) * worldVector;
}

glm::vec3 FlightDynamics::BodyToWorld(const glm::vec3& bodyVector, const glm::quat& orientation) const {
    return orientation * bodyVector;
}

float FlightDynamics::CalculateLiftCoefficient(float angleOfAttack, float elevatorDeflection) const {
    float baseLift = m_aeroCoeffs.CL0 + m_aeroCoeffs.CLa * angleOfAttack;
    float elevatorContribution = m_aeroCoeffs.CLde * elevatorDeflection;
    
    float totalLift = baseLift + elevatorContribution;
    
    // Clamp to maximum lift coefficient (stall condition)
    return std::clamp(totalLift, -m_aeroCoeffs.CLmax, m_aeroCoeffs.CLmax);
}

float FlightDynamics::CalculateDragCoefficient(float liftCoefficient) const {
    // Drag = parasitic drag + induced drag
    float inducedDrag = m_aeroCoeffs.CDi * liftCoefficient * liftCoefficient;
    return m_aeroCoeffs.CD0 + inducedDrag;
}

} // namespace FlightSim 
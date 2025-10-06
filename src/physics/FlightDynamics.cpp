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
    glm::vec3 aeroForces = CalculateAerodynamicForces(state, controls);
    glm::vec3 gravityForce = CalculateGravityForce(state);
    
    forces = aeroForces + gravityForce;
    torques = CalculateAerodynamicTorques(state, controls);
}

glm::vec3 FlightDynamics::CalculateAerodynamicForces(const AircraftState& state, const ControlInputs& controls) {
    glm::vec3 relativeWind = state.velocity - m_environment.windVelocity;
    float airspeed = glm::length(relativeWind);
    
    if (airspeed < 0.1f) {
        return glm::vec3(0.0f);
    }
    
    float airDensity = GetAirDensity(state.altitude);
    float dynamicPressure = 0.5f * airDensity * airspeed * airspeed;
    
    float angleOfAttack = GetAngleOfAttack(state);
    float sideslipAngle = GetSideslipAngle(state);
    
    // MSFS-style lift calculation
    float liftCoeff = CalculateLiftCoefficient(angleOfAttack, controls.elevator);
    liftCoeff += controls.flaps * 0.9f;
    
    // MSFS-style drag calculation
    float dragCoeff = CalculateDragCoefficient(liftCoeff, angleOfAttack);
    dragCoeff += controls.flaps * controls.flaps * 0.12f;
    dragCoeff += controls.brakes * 0.8f;
    
    // Side force from rudder and sideslip
    float sideForceCoeff = m_aeroCoeffs.Cydr * controls.rudder - 0.15f * sideslipAngle;
    
    // Calculate forces
    float lift = liftCoeff * dynamicPressure * m_wingArea;
    float drag = dragCoeff * dynamicPressure * m_wingArea;
    float sideForce = sideForceCoeff * dynamicPressure * m_wingArea;
    
    // Forces in body frame (X: backward drag, Y: side, Z: down/up lift)
    glm::vec3 bodyForces(-drag, sideForce, -lift);
    
    return BodyToWorld(bodyForces, state.orientation);
}

glm::vec3 FlightDynamics::CalculateAerodynamicTorques(const AircraftState& state, const ControlInputs& controls) {
    float airspeed = glm::length(state.velocity);
    
    if (airspeed < 0.1f) {
        return glm::vec3(0.0f);
    }
    
    float dynamicPressure = GetDynamicPressure(state);
    float angleOfAttack = GetAngleOfAttack(state);
    
    // Convert angular velocity to body frame for damping
    glm::vec3 bodyAngVel = state.angularVelocity;
    
    // Pitching moment (elevator + stability + damping)
    float pitchMoment = (m_aeroCoeffs.Cm0 + 
                        m_aeroCoeffs.Cma * angleOfAttack + 
                        m_aeroCoeffs.Cmde * controls.elevator +
                        m_aeroCoeffs.Cmq * bodyAngVel.y * m_wingspan / (2.0f * airspeed)) * 
                       dynamicPressure * m_wingArea * (m_wingspan * 0.25f);
    
    // Rolling moment (aileron + damping)
    float rollMoment = (m_aeroCoeffs.Clda * controls.aileron +
                       m_aeroCoeffs.Clp * bodyAngVel.x * m_wingspan / (2.0f * airspeed)) * 
                      dynamicPressure * m_wingArea * m_wingspan;
    
    // Yawing moment (rudder + adverse yaw + damping)
    float yawMoment = (m_aeroCoeffs.Cndr * controls.rudder +
                      m_aeroCoeffs.Cnda * controls.aileron +
                      m_aeroCoeffs.Cnr * bodyAngVel.z * m_wingspan / (2.0f * airspeed)) * 
                     dynamicPressure * m_wingArea * m_wingspan;
    
    return glm::vec3(rollMoment, pitchMoment, yawMoment);
}

glm::vec3 FlightDynamics::CalculateGravityForce(const AircraftState& state) {
    return glm::vec3(0.0f, -m_mass * 9.81f, 0.0f);
}

glm::vec3 FlightDynamics::CalculateThrustForce(const AircraftState& state, float throttle, float maxThrust) {
    float airDensity = GetAirDensity(state.altitude);
    float densityRatio = airDensity / 1.225f;
    
    // Engine loses power with altitude
    float altitudeFactor = 0.6f + 0.4f * densityRatio;
    float thrust = throttle * maxThrust * altitudeFactor;
    
    glm::vec3 forwardDir = state.orientation * glm::vec3(0.0f, 0.0f, 1.0f);
    return forwardDir * thrust;
}

float FlightDynamics::GetAirDensity(float altitude) const {
    const float seaLevelDensity = 1.225f;
    const float scaleHeight = 8400.0f;
    return seaLevelDensity * exp(-altitude / scaleHeight);
}

float FlightDynamics::GetAngleOfAttack(const AircraftState& state) const {
    glm::vec3 bodyVelocity = WorldToBody(state.velocity, state.orientation);
    
    if (abs(bodyVelocity.z) < 0.1f) {
        return 0.0f;
    }
    
    return atan2(-bodyVelocity.y, bodyVelocity.z);
}

float FlightDynamics::GetSideslipAngle(const AircraftState& state) const {
    glm::vec3 bodyVelocity = WorldToBody(state.velocity, state.orientation);
    float velocityMagnitude = glm::length(bodyVelocity);
    
    if (velocityMagnitude < 0.1f) {
        return 0.0f;
    }
    
    return asin(std::clamp(bodyVelocity.x / velocityMagnitude, -1.0f, 1.0f));
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
    float elevatorLift = m_aeroCoeffs.CLde * elevatorDeflection;
    float totalLift = baseLift + elevatorLift;
    
    // Realistic stall behavior
    const float stallAnglePositive = 0.26f;  // ~15 degrees
    const float stallAngleNegative = -0.22f; // ~-13 degrees
    
    if (angleOfAttack > stallAnglePositive) {
        float excessAngle = angleOfAttack - stallAnglePositive;
        totalLift = m_aeroCoeffs.CLmax * (1.0f - excessAngle * 4.0f);
        totalLift = std::max(totalLift, 0.3f);
    } else if (angleOfAttack < stallAngleNegative) {
        float excessAngle = abs(angleOfAttack - stallAngleNegative);
        totalLift = m_aeroCoeffs.CLmin * (1.0f - excessAngle * 4.0f);
        totalLift = std::min(totalLift, -0.3f);
    }
    
    return std::clamp(totalLift, m_aeroCoeffs.CLmin, m_aeroCoeffs.CLmax);
}

float FlightDynamics::CalculateDragCoefficient(float liftCoefficient, float angleOfAttack) const {
    // Parasite drag + induced drag
    float inducedDrag = m_aeroCoeffs.CDi * liftCoefficient * liftCoefficient;
    float baseDrag = m_aeroCoeffs.CD0 + inducedDrag;
    
    // Extra drag in stall
    if (abs(angleOfAttack) > 0.26f) {
        baseDrag += abs(angleOfAttack - 0.26f) * 8.0f;
    }
    
    return std::min(baseDrag, m_aeroCoeffs.CDmax);
}

} // namespace FlightSim

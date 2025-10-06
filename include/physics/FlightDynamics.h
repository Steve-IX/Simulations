#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace FlightSim {

struct ControlInputs;
struct AircraftState;

// MSFS-Style Aerodynamic Coefficients
struct AerodynamicCoefficients {
    // Lift coefficients
    float CL0 = 0.28f;       // Zero AoA lift coefficient
    float CLa = 5.5f;        // Lift curve slope (per radian)
    float CLmax = 1.6f;      // Maximum lift coefficient
    float CLmin = -1.2f;     // Minimum lift coefficient (negative stall)
    
    // Drag coefficients
    float CD0 = 0.025f;      // Zero-lift drag (parasite drag)
    float CDi = 0.045f;      // Induced drag factor
    float CDmax = 2.0f;      // Maximum drag (stall)
    
    // Moment coefficients
    float Cm0 = -0.02f;      // Zero AoA moment coefficient
    float Cma = -0.6f;       // Pitch stability derivative
    float Cmq = -15.0f;      // Pitch damping
    
    // Control effectiveness
    float CLde = 0.5f;       // Elevator lift effectiveness
    float Cmde = -1.5f;      // Elevator moment effectiveness
    float Cydr = 0.35f;      // Rudder side force
    float Cndr = -0.12f;     // Rudder yaw moment
    float Clda = 0.25f;      // Aileron roll moment
    float Cnda = -0.02f;     // Adverse yaw from ailerons
    
    // Damping coefficients
    float Clp = -0.5f;       // Roll damping
    float Cnr = -0.3f;       // Yaw damping
};

struct EnvironmentData {
    float airDensity = 1.225f;
    glm::vec3 windVelocity{0.0f};
    float temperature = 288.15f;
    float pressure = 101325.0f;
};

class FlightDynamics {
public:
    FlightDynamics();
    
    void SetAircraftParameters(float mass, float wingArea, float wingspan, const glm::mat3& inertiaTensor);
    void SetAerodynamicCoefficients(const AerodynamicCoefficients& coeffs);
    
    // Main physics calculation - MSFS style
    void CalculateForces(const AircraftState& state, const ControlInputs& controls, 
                        glm::vec3& forces, glm::vec3& torques);
    
    // Individual force calculations
    glm::vec3 CalculateAerodynamicForces(const AircraftState& state, const ControlInputs& controls);
    glm::vec3 CalculateAerodynamicTorques(const AircraftState& state, const ControlInputs& controls);
    glm::vec3 CalculateGravityForce(const AircraftState& state);
    glm::vec3 CalculateThrustForce(const AircraftState& state, float throttle, float maxThrust);
    
    // Environment
    void SetEnvironment(const EnvironmentData& env) { m_environment = env; }
    float GetAirDensity(float altitude) const;
    
    // Utility functions
    float GetAngleOfAttack(const AircraftState& state) const;
    float GetSideslipAngle(const AircraftState& state) const;
    float GetDynamicPressure(const AircraftState& state) const;
    
private:
    float m_mass;
    float m_wingArea;
    float m_wingspan;
    glm::mat3 m_inertiaTensor;
    
    AerodynamicCoefficients m_aeroCoeffs;
    EnvironmentData m_environment;
    
    // Helper functions
    glm::vec3 WorldToBody(const glm::vec3& worldVector, const glm::quat& orientation) const;
    glm::vec3 BodyToWorld(const glm::vec3& bodyVector, const glm::quat& orientation) const;
    float CalculateLiftCoefficient(float angleOfAttack, float elevatorDeflection) const;
    float CalculateDragCoefficient(float liftCoefficient, float angleOfAttack) const;
};

} // namespace FlightSim

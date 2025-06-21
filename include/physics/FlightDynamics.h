#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace FlightSim {

struct ControlInputs;
struct AircraftState;

struct AerodynamicCoefficients {
    // Lift coefficients
    float CL0 = 0.4f;        // Zero angle of attack lift coefficient
    float CLa = 5.7f;        // Lift curve slope (per radian)
    float CLmax = 1.4f;      // Maximum lift coefficient
    
    // Drag coefficients
    float CD0 = 0.03f;       // Zero-lift drag coefficient
    float CDi = 0.04f;       // Induced drag factor
    
    // Moment coefficients
    float Cm0 = -0.1f;       // Zero angle of attack moment coefficient
    float Cma = -0.8f;       // Moment curve slope (stability)
    
    // Control surface effectiveness
    float CLde = 0.4f;       // Elevator lift effectiveness
    float Cmde = -1.2f;      // Elevator moment effectiveness
    float Cydr = 0.3f;       // Rudder side force effectiveness
    float Cndr = -0.1f;      // Rudder yaw moment effectiveness
    float Clda = 0.2f;       // Aileron roll moment effectiveness
};

struct EnvironmentData {
    float airDensity = 1.225f;     // kg/m³ at sea level
    glm::vec3 windVelocity{0.0f};  // m/s
    float temperature = 288.15f;    // Kelvin at sea level
    float pressure = 101325.0f;     // Pa at sea level
};

class FlightDynamics {
public:
    FlightDynamics();
    
    void SetAircraftParameters(float mass, float wingArea, float wingspan, const glm::mat3& inertiaTensor);
    void SetAerodynamicCoefficients(const AerodynamicCoefficients& coeffs);
    
    // Main physics calculation
    void CalculateForces(const AircraftState& state, const ControlInputs& controls, 
                        glm::vec3& forces, glm::vec3& torques);
    
    // Individual force calculations
    glm::vec3 CalculateAerodynamicForces(const AircraftState& state, const ControlInputs& controls);
    glm::vec3 CalculateAerodynamicTorques(const AircraftState& state, const ControlInputs& controls);
    glm::vec3 CalculateGravityForce(const AircraftState& state);
    glm::vec3 CalculateThrustForce(const AircraftState& state, float throttle, float maxThrust);
    
    // Environment effects
    void SetEnvironment(const EnvironmentData& env) { m_environment = env; }
    float GetAirDensity(float altitude) const;
    
    // Utility functions
    float GetAngleOfAttack(const AircraftState& state) const;
    float GetSideslipAngle(const AircraftState& state) const;
    float GetDynamicPressure(const AircraftState& state) const;
    
private:
    // Aircraft parameters
    float m_mass;
    float m_wingArea;
    float m_wingspan;
    glm::mat3 m_inertiaTensor;
    
    // Aerodynamic coefficients
    AerodynamicCoefficients m_aeroCoeffs;
    
    // Environment
    EnvironmentData m_environment;
    
    // Helper functions
    glm::vec3 WorldToBody(const glm::vec3& worldVector, const glm::quat& orientation) const;
    glm::vec3 BodyToWorld(const glm::vec3& bodyVector, const glm::quat& orientation) const;
    float CalculateLiftCoefficient(float angleOfAttack, float elevatorDeflection) const;
    float CalculateDragCoefficient(float liftCoefficient) const;
};

} // namespace FlightSim 
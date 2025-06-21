#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "../physics/Aircraft.h"

namespace FlightSim {

class Window;

class InputManager {
public:
    InputManager();
    ~InputManager();
    
    bool Initialize(Window* window);
    void Shutdown();
    
    // Input processing
    void ProcessKeyboard(int key, int scancode, int action, int mods);
    void ProcessMouse(double xpos, double ypos);
    void ProcessScroll(double xoffset, double yoffset);
    
    // Input queries
    bool IsKeyPressed(int key) const;
    bool IsKeyHeld(int key) const;
    glm::vec2 GetMousePosition() const;
    glm::vec2 GetMouseOffset() const;
    glm::vec2 GetScrollOffset() const;
    
    // Flight control inputs
    ControlInputs GetControlInputs() const;
    
    // Joystick support
    void UpdateJoystick();
    bool HasJoystick() const { return m_joystickPresent; }
    
private:
    void UpdateControlInputs();
    void ResetInputs();
    
    Window* m_window;
    
    // Keyboard state
    bool m_keys[GLFW_KEY_LAST];
    bool m_keysPressed[GLFW_KEY_LAST];
    
    // Mouse state
    glm::vec2 m_lastMousePos;
    glm::vec2 m_mouseOffset;
    glm::vec2 m_scrollOffset;
    bool m_firstMouse;
    
    // Joystick state
    bool m_joystickPresent;
    int m_joystickID;
    
    // Control inputs
    mutable ControlInputs m_controlInputs;
    
    // Input sensitivity settings
    float m_mouseSensitivity;
    float m_keyboardSensitivity;
    float m_joystickDeadzone;
    
    // Control mapping
    void ProcessFlightControls() const;
    void ProcessThrottleControls() const;
    void ProcessAuxiliaryControls() const;
};

} // namespace FlightSim 
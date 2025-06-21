#include "input/InputManager.h"
#include "core/Window.h"
#include <cstring>
#include <algorithm>

namespace FlightSim {

InputManager::InputManager()
    : m_window(nullptr)
    , m_lastMousePos(0.0f)
    , m_mouseOffset(0.0f)
    , m_scrollOffset(0.0f)
    , m_firstMouse(true)
    , m_joystickPresent(false)
    , m_joystickID(GLFW_JOYSTICK_1)
    , m_mouseSensitivity(0.1f)
    , m_keyboardSensitivity(1.0f)
    , m_joystickDeadzone(0.1f) {
    
    std::memset(m_keys, 0, sizeof(m_keys));
    std::memset(m_keysPressed, 0, sizeof(m_keysPressed));
}

InputManager::~InputManager() {
    Shutdown();
}

bool InputManager::Initialize(Window* window) {
    m_window = window;
    
    // Check for joystick
    UpdateJoystick();
    
    return true;
}

void InputManager::Shutdown() {
    m_window = nullptr;
}

void InputManager::ProcessKeyboard(int key, int scancode, int action, int mods) {
    if (key >= 0 && key < GLFW_KEY_LAST) {
        bool wasPressed = m_keys[key];
        m_keys[key] = (action == GLFW_PRESS || action == GLFW_REPEAT);
        m_keysPressed[key] = (action == GLFW_PRESS && !wasPressed);
    }
}

void InputManager::ProcessMouse(double xpos, double ypos) {
    if (m_firstMouse) {
        m_lastMousePos = glm::vec2(xpos, ypos);
        m_firstMouse = false;
    }
    
    m_mouseOffset.x = xpos - m_lastMousePos.x;
    m_mouseOffset.y = m_lastMousePos.y - ypos; // Reversed since y-coordinates go from bottom to top
    
    m_lastMousePos = glm::vec2(xpos, ypos);
}

void InputManager::ProcessScroll(double xoffset, double yoffset) {
    m_scrollOffset = glm::vec2(xoffset, yoffset);
}

ControlInputs InputManager::GetControlInputs() const {
    ProcessFlightControls();
    ProcessThrottleControls();
    ProcessAuxiliaryControls();
    
    return m_controlInputs;
}

bool InputManager::IsKeyPressed(int key) const {
    if (key >= 0 && key < GLFW_KEY_LAST) {
        return m_keys[key];
    }
    return false;
}

bool InputManager::IsKeyHeld(int key) const {
    if (key >= 0 && key < GLFW_KEY_LAST) {
        return m_keys[key];
    }
    return false;
}

glm::vec2 InputManager::GetMousePosition() const {
    return m_lastMousePos;
}

glm::vec2 InputManager::GetMouseOffset() const {
    return m_mouseOffset;
}

glm::vec2 InputManager::GetScrollOffset() const {
    return m_scrollOffset;
}

void InputManager::UpdateJoystick() {
    m_joystickPresent = glfwJoystickPresent(m_joystickID);
    
    if (m_joystickPresent) {
        const char* name = glfwGetJoystickName(m_joystickID);
        // Could log joystick info here
    }
}

void InputManager::UpdateControlInputs() {
    // This method could be used for more complex input processing
    // Currently implemented directly in GetControlInputs()
}

void InputManager::ResetInputs() {
    m_mouseOffset = glm::vec2(0.0f);
    m_scrollOffset = glm::vec2(0.0f);
    std::memset(m_keysPressed, 0, sizeof(m_keysPressed));
}

void InputManager::ProcessFlightControls() const {
    // Pitch control (W/S keys)
    if (m_keys[GLFW_KEY_W]) {
        m_controlInputs.elevator = std::min(m_controlInputs.elevator + 0.02f, 1.0f);
    }
    if (m_keys[GLFW_KEY_S]) {
        m_controlInputs.elevator = std::max(m_controlInputs.elevator - 0.02f, -1.0f);
    }
    
    // Roll control (A/D keys)
    if (m_keys[GLFW_KEY_A]) {
        m_controlInputs.aileron = std::max(m_controlInputs.aileron - 0.02f, -1.0f);
    }
    if (m_keys[GLFW_KEY_D]) {
        m_controlInputs.aileron = std::min(m_controlInputs.aileron + 0.02f, 1.0f);
    }
    
    // Yaw control (Q/E keys)
    if (m_keys[GLFW_KEY_Q]) {
        m_controlInputs.rudder = std::max(m_controlInputs.rudder - 0.02f, -1.0f);
    }
    if (m_keys[GLFW_KEY_E]) {
        m_controlInputs.rudder = std::min(m_controlInputs.rudder + 0.02f, 1.0f);
    }
    
    // Return controls to center when not pressed
    if (!m_keys[GLFW_KEY_W] && !m_keys[GLFW_KEY_S]) {
        m_controlInputs.elevator *= 0.95f;
    }
    if (!m_keys[GLFW_KEY_A] && !m_keys[GLFW_KEY_D]) {
        m_controlInputs.aileron *= 0.95f;
    }
    if (!m_keys[GLFW_KEY_Q] && !m_keys[GLFW_KEY_E]) {
        m_controlInputs.rudder *= 0.95f;
    }
}

void InputManager::ProcessThrottleControls() const {
    // Throttle control (Shift/Ctrl keys)
    if (m_keys[GLFW_KEY_LEFT_SHIFT]) {
        m_controlInputs.throttle = std::min(m_controlInputs.throttle + 0.01f, 1.0f);
    }
    if (m_keys[GLFW_KEY_LEFT_CONTROL]) {
        m_controlInputs.throttle = std::max(m_controlInputs.throttle - 0.01f, 0.0f);
    }
}

void InputManager::ProcessAuxiliaryControls() const {
    // These would be handled with key press events, not continuous input
    // For now, we'll leave them as placeholder
}

} // namespace FlightSim 
#include "core/Window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace FlightSim {

Window::Window(int width, int height, const std::string& title)
    : m_window(nullptr), m_width(width), m_height(height), m_title(title) {
}

Window::~Window() {
    Shutdown();
}

bool Window::Initialize() {
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create window
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return false;
    }
    
    glfwMakeContextCurrent(m_window);
    
    // Set user pointer for callbacks
    glfwSetWindowUserPointer(m_window, this);
    
    // Set callbacks
    glfwSetKeyCallback(m_window, KeyCallbackWrapper);
    glfwSetCursorPosCallback(m_window, MouseCallbackWrapper);
    glfwSetScrollCallback(m_window, ScrollCallbackWrapper);
    glfwSetFramebufferSizeCallback(m_window, ResizeCallbackWrapper);
    
    return true;
}

void Window::Shutdown() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::SwapBuffers() {
    glfwSwapBuffers(m_window);
}

void Window::PollEvents() {
    glfwPollEvents();
}

void Window::SetKeyCallback(KeyCallback callback) {
    m_keyCallback = callback;
}

void Window::SetMouseCallback(MouseCallback callback) {
    m_mouseCallback = callback;
}

void Window::SetScrollCallback(ScrollCallback callback) {
    m_scrollCallback = callback;
}

void Window::SetResizeCallback(ResizeCallback callback) {
    m_resizeCallback = callback;
}

void Window::SetVSync(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
}

void Window::SetCursorMode(int mode) {
    glfwSetInputMode(m_window, GLFW_CURSOR, mode);
}

// Static callback wrappers
void Window::KeyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window* windowPtr = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (windowPtr && windowPtr->m_keyCallback) {
        windowPtr->m_keyCallback(key, scancode, action, mods);
    }
}

void Window::MouseCallbackWrapper(GLFWwindow* window, double xpos, double ypos) {
    Window* windowPtr = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (windowPtr && windowPtr->m_mouseCallback) {
        windowPtr->m_mouseCallback(xpos, ypos);
    }
}

void Window::ScrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset) {
    Window* windowPtr = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (windowPtr && windowPtr->m_scrollCallback) {
        windowPtr->m_scrollCallback(xoffset, yoffset);
    }
}

void Window::ResizeCallbackWrapper(GLFWwindow* window, int width, int height) {
    Window* windowPtr = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (windowPtr) {
        windowPtr->m_width = width;
        windowPtr->m_height = height;
        if (windowPtr->m_resizeCallback) {
            windowPtr->m_resizeCallback(width, height);
        }
    }
}

} // namespace FlightSim 
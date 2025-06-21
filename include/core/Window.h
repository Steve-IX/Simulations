#pragma once

#include <string>
#include <functional>

// Forward declarations
struct GLFWwindow;

namespace FlightSim {

class Window {
public:
    using KeyCallback = std::function<void(int, int, int, int)>;
    using MouseCallback = std::function<void(double, double)>;
    using ScrollCallback = std::function<void(double, double)>;
    using ResizeCallback = std::function<void(int, int)>;
    
public:
    Window(int width, int height, const std::string& title);
    ~Window();
    
    bool Initialize();
    void Shutdown();
    
    bool ShouldClose() const;
    void SwapBuffers();
    void PollEvents();
    
    void SetKeyCallback(KeyCallback callback);
    void SetMouseCallback(MouseCallback callback);
    void SetScrollCallback(ScrollCallback callback);
    void SetResizeCallback(ResizeCallback callback);
    
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    GLFWwindow* GetHandle() const { return m_window; }
    
    void SetVSync(bool enabled);
    void SetCursorMode(int mode);
    
private:
    static void KeyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset);
    static void ResizeCallbackWrapper(GLFWwindow* window, int width, int height);
    
    GLFWwindow* m_window;
    int m_width, m_height;
    std::string m_title;
    
    KeyCallback m_keyCallback;
    MouseCallback m_mouseCallback;
    ScrollCallback m_scrollCallback;
    ResizeCallback m_resizeCallback;
};

} // namespace FlightSim 
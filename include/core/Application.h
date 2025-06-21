#pragma once

#include <memory>
#include <string>

// Forward declarations
struct GLFWwindow;

namespace FlightSim {

class Window;
class Renderer;
class Camera;
class Aircraft;
class InputManager;
class HUD;

class Application {
public:
    Application();
    ~Application();
    
    bool Initialize();
    void Run();
    void Shutdown();
    
private:
    void Update(float deltaTime);
    void Render();
    void HandleInput(float deltaTime);
    
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Aircraft> m_aircraft;
    std::unique_ptr<InputManager> m_inputManager;
    std::unique_ptr<HUD> m_hud;
    
    bool m_running;
    bool m_initialized;
    
    // Timing
    float m_lastFrameTime;
    float m_deltaTime;
};

} // namespace FlightSim 
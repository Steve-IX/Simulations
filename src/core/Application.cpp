#include "core/Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>

#include "core/Window.h"
#include "core/Camera.h"
#include "renderer/Renderer.h"
#include "physics/Aircraft.h"
#include "input/InputManager.h"
#include "ui/HUD.h"

namespace FlightSim {

// Application settings
static constexpr int WINDOW_WIDTH = 1280;
static constexpr int WINDOW_HEIGHT = 720;
static constexpr const char* WINDOW_TITLE = "Professional Flight Simulator v1.0";

Application::Application()
    : m_running(false)
    , m_initialized(false)
    , m_lastFrameTime(0.0f)
    , m_deltaTime(0.0f) {
}

Application::~Application() {
    Shutdown();
}

bool Application::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Create window
    m_window = std::make_unique<Window>(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    if (!m_window->Initialize()) {
        std::cerr << "Failed to create window" << std::endl;
        return false;
    }
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    // Create subsystems
    m_camera = std::make_unique<Camera>();
    m_renderer = std::make_unique<Renderer>();
    m_aircraft = std::make_unique<Aircraft>();
    m_inputManager = std::make_unique<InputManager>();
    m_hud = std::make_unique<HUD>();
    
    // Initialize subsystems
    if (!m_renderer->Initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }
    
    if (!m_hud->Initialize()) {
        std::cerr << "Failed to initialize HUD" << std::endl;
        return false;
    }
    
    // Set up input callbacks
    m_window->SetKeyCallback([this](int key, int scancode, int action, int mods) {
        m_inputManager->ProcessKeyboard(key, scancode, action, mods);
    });
    
    m_window->SetMouseCallback([this](double xpos, double ypos) {
        m_inputManager->ProcessMouse(xpos, ypos);
    });
    
    m_window->SetScrollCallback([this](double xoffset, double yoffset) {
        m_inputManager->ProcessScroll(xoffset, yoffset);
    });
    
    m_window->SetResizeCallback([this](int width, int height) {
        glViewport(0, 0, width, height);
        m_camera->SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    });
    
    // Set initial camera aspect ratio
    m_camera->SetAspectRatio(static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT));
    
    m_initialized = true;
    return true;
}

void Application::Run() {
    if (!m_initialized) {
        std::cerr << "Application not initialized" << std::endl;
        return;
    }
    
    m_running = true;
    m_lastFrameTime = static_cast<float>(glfwGetTime());
    
    while (m_running && !m_window->ShouldClose()) {
        float currentTime = static_cast<float>(glfwGetTime());
        m_deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;
        
        m_window->PollEvents();
        HandleInput(m_deltaTime);
        Update(m_deltaTime);
        Render();
        m_window->SwapBuffers();
    }
}

void Application::Update(float deltaTime) {
    // Get control inputs
    ControlInputs inputs = m_inputManager->GetControlInputs();
    
    // Update aircraft physics
    m_aircraft->Update(deltaTime, inputs);
    
    // Update camera
    m_camera->Update(*m_aircraft, deltaTime);
    
    // Update HUD
    m_hud->Update(m_aircraft->GetState(), deltaTime);
}

void Application::Render() {
    m_renderer->BeginFrame();
    m_renderer->RenderScene(*m_camera, *m_aircraft);
    m_hud->Render(*m_camera, m_aircraft->GetState());
    m_renderer->EndFrame();
}

void Application::HandleInput(float deltaTime) {
    // Handle application-level input
    if (m_inputManager->IsKeyPressed(GLFW_KEY_ESCAPE)) {
        m_running = false;
    }
    
    // Handle camera mode switching
    static bool cKeyPressed = false;
    if (m_inputManager->IsKeyPressed(GLFW_KEY_C) && !cKeyPressed) {
        m_camera->CycleMode();
        cKeyPressed = true;
    } else if (!m_inputManager->IsKeyPressed(GLFW_KEY_C)) {
        cKeyPressed = false;
    }
    
    // Handle aircraft reset
    static bool rKeyPressed = false;
    if (m_inputManager->IsKeyPressed(GLFW_KEY_R) && !rKeyPressed) {
        m_aircraft->Reset();
        rKeyPressed = true;
    } else if (!m_inputManager->IsKeyPressed(GLFW_KEY_R)) {
        rKeyPressed = false;
    }
}

void Application::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    m_hud.reset();
    m_inputManager.reset();
    m_aircraft.reset();
    m_renderer.reset();
    m_camera.reset();
    m_window.reset();
    
    glfwTerminate();
    m_initialized = false;
}

} // namespace FlightSim 
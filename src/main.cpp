#include <iostream>
#include <exception>

#include "core/Application.h"

int main() {
    try {
        FlightSim::Application app;
        
        if (!app.Initialize()) {
            std::cerr << "Failed to initialize Flight Simulator" << std::endl;
            return -1;
        }
        
        std::cout << "Professional Flight Simulator v1.0 - Starting..." << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  W/S: Pitch (Elevator)" << std::endl;
        std::cout << "  A/D: Roll (Aileron)" << std::endl;
        std::cout << "  Q/E: Yaw (Rudder)" << std::endl;
        std::cout << "  Shift/Ctrl: Throttle" << std::endl;
        std::cout << "  F: Toggle Flaps" << std::endl;
        std::cout << "  G: Toggle Landing Gear" << std::endl;
        std::cout << "  C: Change Camera Mode" << std::endl;
        std::cout << "  R: Reset Aircraft" << std::endl;
        std::cout << "  ESC: Exit" << std::endl;
        std::cout << std::endl;
        
        app.Run();
        
        app.Shutdown();
        std::cout << "Flight Simulator shutdown complete." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
} 
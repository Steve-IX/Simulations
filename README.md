# Professional Flight Simulator

A realistic flight simulator built with modern C++ and OpenGL, featuring accurate flight physics, multiple camera modes, and professional flight instrumentation

## Features

### Core Features
- **Realistic Flight Physics**: Six-degree-of-freedom flight model with accurate aerodynamics
- **Multiple Aircraft**: Configurable aircraft parameters for different flight characteristics
- **Professional Graphics**: Modern OpenGL 3.3+ rendering with lighting and fog effects
- **Multiple Camera Modes**: Cockpit, External, Chase, and Free camera modes
- **Flight Instrumentation**: Complete HUD with altitude, speed, attitude, and navigation indicators

### Flight Controls
- **Primary Controls**: Elevator, Aileron, Rudder control via keyboard or joystick
- **Engine Controls**: Variable throttle control
- **Auxiliary Systems**: Flaps, landing gear, brakes
- **Camera Control**: Switch between different viewing perspectives

### Technical Features
- **Modern C++ Architecture**: Clean, modular design with proper RAII and smart pointers
- **Cross-Platform**: Windows, macOS, and Linux support
- **Performance Optimized**: Efficient rendering and physics calculations
- **Extensible Design**: Easy to add new aircraft types and features

## Prerequisites

### Required Dependencies
- **CMake** 3.16 or higher
- **OpenGL** 3.3 or higher
- **GLFW3** (for window management)
- **GLM** (for mathematics)
- **GLAD** (for OpenGL loading)

### Build Tools
- **GCC 7+** or **Clang 7+** or **MSVC 2019+**
- **C++17** compatible compiler

## Building

### Windows (Visual Studio)
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

### Windows (MinGW)
```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
make -j4
```

### Linux/macOS
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## Controls

### Flight Controls
| Key | Action |
|-----|--------|
| W/S | Pitch (Elevator) |
| A/D | Roll (Aileron) |
| Q/E | Yaw (Rudder) |
| Shift/Ctrl | Throttle Up/Down |
| F | Toggle Flaps |
| G | Toggle Landing Gear |
| B | Brakes |

### Camera Controls
| Key | Action |
|-----|--------|
| C | Cycle Camera Modes |
| Mouse | Look Around (Free Camera) |
| Scroll | Zoom In/Out |

### General Controls
| Key | Action |
|-----|--------|
| R | Reset Aircraft |
| ESC | Exit |

## Flight Physics

The simulator implements a comprehensive flight dynamics model including:

- **Aerodynamic Forces**: Lift, drag, and side forces based on angle of attack and control inputs
- **Moments**: Roll, pitch, and yaw moments for realistic aircraft response
- **Environmental Effects**: Altitude-dependent air density and atmospheric conditions
- **Engine Modeling**: Thrust vectoring and power management
- **Ground Effects**: Basic ground collision and physics

## Architecture

### Core Systems
- **Application**: Main application loop and system coordination
- **Window**: GLFW-based window management with event handling
- **Renderer**: OpenGL-based 3D rendering system
- **Camera**: Multi-mode camera system with smooth transitions

### Physics Systems
- **Aircraft**: Complete aircraft state management and integration
- **FlightDynamics**: Aerodynamic calculations and force generation
- **Environmental**: Atmospheric modeling and weather effects

### Input/Output Systems
- **InputManager**: Unified input handling for keyboard, mouse, and joystick
- **HUD**: Professional flight instrumentation display

## Project Structure

```
FlightSimulator/
├── CMakeLists.txt              # Main build configuration
├── README.md                   # This file
├── include/                    # Header files
│   ├── core/                   # Core engine headers
│   ├── physics/                # Physics system headers
│   ├── renderer/               # Rendering system headers
│   ├── input/                  # Input handling headers
│   └── ui/                     # User interface headers
├── src/                        # Source files
│   ├── main.cpp                # Application entry point
│   ├── core/                   # Core engine implementation
│   ├── physics/                # Physics implementation
│   ├── renderer/               # Rendering implementation
│   ├── input/                  # Input handling implementation
│   └── ui/                     # User interface implementation
├── resources/                  # Game resources
│   └── shaders/                # GLSL shader files
└── external/                   # Third-party dependencies
    ├── glad/                   # OpenGL loader
    ├── glm/                    # Math library
    └── stb/                    # Image loading
```

## Configuration

### Aircraft Parameters
The aircraft behavior can be customized by modifying parameters in the `Aircraft` class:
- Mass and inertia properties
- Wing area and aerodynamic coefficients
- Engine thrust and fuel consumption
- Control surface effectiveness

### Graphics Settings
Rendering options can be adjusted in the `Renderer` class:
- Lighting parameters
- Fog density and color
- Shadow quality
- Anti-aliasing settings

## Performance

### System Recommendations
- **CPU**: Intel i5-8400 / AMD Ryzen 5 2600 or better
- **GPU**: NVIDIA GTX 1060 / AMD RX 580 or better
- **RAM**: 8GB or more
- **Storage**: 100MB available space

### Performance Features
- Efficient culling and LOD systems
- Optimized physics calculations
- Smart resource management
- Configurable quality settings

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

### Development Setup
1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## Acknowledgments

- OpenGL community for excellent documentation and tutorials
- GLFW team for the cross-platform windowing library
- GLM developers for the comprehensive math library
- Flight simulation community for aerodynamic modeling references 

cmake_minimum_required(VERSION 3.16)
project(FlightSimulator VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compile options
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external/glad/include)
include_directories(${CMAKE_SOURCE_DIR}/external/glm)
include_directories(${CMAKE_SOURCE_DIR}/external/stb)

# Add subdirectories
add_subdirectory(external)
add_subdirectory(src)

# Create executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/core/Application.cpp
    src/core/Window.cpp
    src/core/Shader.cpp
    src/core/Camera.cpp
    src/core/Mesh.cpp
    src/physics/Aircraft.cpp
    src/physics/FlightDynamics.cpp
    src/renderer/Renderer.cpp
    src/renderer/SkyBox.cpp
    src/renderer/Terrain.cpp
    src/input/InputManager.cpp
    src/ui/HUD.cpp
    external/glad/src/glad.c
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    glfw
)

# Include directories for target
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/stb
)

# Copy resources
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR}) 
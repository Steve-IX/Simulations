@echo off
echo Building Professional Flight Simulator...

if not exist build mkdir build
cd build

echo Configuring with CMake...
cmake .. -G "MinGW Makefiles"

if %errorlevel% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b %errorlevel%
)

echo Building project...
mingw32-make -j%NUMBER_OF_PROCESSORS%

if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b %errorlevel%
)

echo Build completed successfully!
echo.
echo Run FlightSimulator.exe to start the flight simulator.
pause 
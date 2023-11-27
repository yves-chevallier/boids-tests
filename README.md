
## Building on Linux

Easy on Linux/Ubuntu:

```bash
git clone https://github.com/yves-chevallier/boids-tests.git
cd boids-tests
sudo apt install -y libsfml-dev libtgui-dev libglew-dev libgl1-mesa-dev libglu1-mesa-dev
sudo apt install -y make cmake build-essential
mkdir build && cd build
cmake ..
make
```

On WSL, the geometry shader is not available unfortunately. I did not find a solution to run it. However it is possible to build on Windows.

## Building on Windows

This project can be built on Windows using Visual Studio 2022 and vcpkg for dependency management. You need to install the 20GB Visual Studio Community before...

### Prerequisites

- [Visual Studio 2022](https://visualstudio.microsoft.com/vs/)
- [vcpkg](https://github.com/Microsoft/vcpkg)
- Git

### Steps

1. **Clone the Repository**
   ```bash
   git clone https://github.com/yves-chevallier/boids-tests.git
   cd boids-tests
   ```

2. **Install Dependencies with vcpkg**

   - Install vcpkg following [these instructions](https://github.com/Microsoft/vcpkg#quick-start-windows).
   - Install required libraries:
   ```
   .\vcpkg\vcpkg install sfml tgui glew
   ```

3. **Configure Visual Studio with vcpkg**
   - Integrate vcpkg with Visual Studio:
     ```
     .\vcpkg\vcpkg integrate install
     ```
   - Open the project in Visual Studio.
   - Set the vcpkg toolchain in CMake settings (Project > CMake Settings for [your project]) to `[path to vcpkg]/scripts/buildsystems/vcpkg.cmake`.

4. **Build and Run the Project**
   - In Visual Studio, right-click on `CMakeLists.txt` and select **Build All**.
   - Run the application by setting it as the startup item and pressing `F5`.

### Troubleshooting

- If you encounter any issues, first ensure all dependencies are correctly installed and accessible to Visual Studio.
- Clearing the CMake cache (CMake Cache > delete) can resolve many configuration issues.

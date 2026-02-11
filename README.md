# Lorenz Attractor Visualization - C++/OpenGL

A decently performing real-time 3D visualization of the Lorenz chaotic attractor, implemented in modern C++17 with OpenGL 4.2. Achieves **116+ FPS** with **15,000+ trajectory points**, representing a **30-100× performance improvement** over Python implementations.

![Lorenz Attractor](https://www.youtube.com/watch?v=m2MSvsjyEuw) _The iconic butterfly-shaped strange attractor with velocity-based colour gradients_

## Features

- **Real-time chaos simulation** using Runge-Kutta 4th order (RK4) numerical integration
- **Hardware-accelerated rendering** with OpenGL vertex/fragment shaders
- **Interactive 3D camera** with mouse controls (orbit, pan, zoom)
- **Dynamic color gradients** based on trajectory height and velocity
- **Live parameter tuning** via ImGui interface
- **Numerical precision analysis** - demonstrates why `-ffast-math` breaks chaos theory
- **Optimized performance** - 95% GPU utilization, minimal CPU overhead

## Performance Metrics

|Metric|Value|
|---|---|
|Frame Rate|116+ FPS|
|Trajectory Points|15,000+|
|Physics Computation|~2% frame time|
|Rendering|~5% frame time|
|Speedup vs Python|30-100×|

##  Project Motivation

This project explores the intersection of:

- **Chaos Theory**: Understanding deterministic yet unpredictable dynamical systems
- **Numerical Methods**: Implementing accurate ODE solvers for sensitive systems
- **Graphics Programming**: Real-time GPU-accelerated visualization
- **Performance Optimization**: Systematic bottleneck analysis and resolution

##  Technical Stack

- **Language**: C++17
- **Graphics**: OpenGL 4.2 Core Profile
- **Math Library**: GLM (OpenGL Mathematics)
- **Window Management**: GLFW3
- **OpenGL Loader**: GLAD
- **GUI**: ImGui (optional)
- **Build System**: CMake 3.15+

##  Prerequisites

### Ubuntu/Debian

```bash
sudo apt update
sudo apt install build-essential cmake
sudo apt install libglfw3-dev libglm-dev
sudo apt install mesa-utils libgl1-mesa-dev
```

### Fedora/RHEL

```bash
sudo dnf install gcc-c++ cmake
sudo dnf install glfw-devel glm-devel
sudo dnf install mesa-libGL-devel
```

### macOS

```bash
brew install cmake glfw glm
```

### Windows (MSYS2/MinGW)

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-glfw mingw-w64-x86_64-glm
```

##  Installation

### 1. Clone the Repository

```bash
git clone https://github.com/yourusername/lorenz-opengl.git
cd lorenz-opengl
```

### 2. Download External Dependencies

#### GLAD (OpenGL Function Loader)

1. Visit [GLAD Generator](https://glad.dav1d.de/)
2. Set options:
    - **Language**: C/C++
    - **Specification**: OpenGL
    - **gl**: Version 4.2 (or higher)
    - **Profile**: Core
3. Click **Generate**
4. Download and extract to `external/glad/`

Expected structure:

```
external/glad/
├── include/
│   ├── glad/
│   │   └── glad.h
│   └── KHR/
│       └── khrplatform.h
└── src/
    └── glad.c
```

#### ImGui (Optional - for GUI controls)

```bash
cd external/
git clone https://github.com/ocornut/imgui.git
cd imgui
git checkout v1.89.9  # Or latest stable version
```

**Note**: GUI controls are optional. Without ImGui, the program uses keyboard controls only.

### 3. Build the Project

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### 4. Run

```bash
./lorenz_viz
```

## 🎮 Controls

### Keyboard

|Key|Action|
|---|---|
|`SPACE`|Start/Stop simulation|
|`R`|Reset camera to default view|
|`ESC`|Exit application|

### Mouse

|Input|Action|
|---|---|
|Left Click + Drag|Rotate camera (orbit)|
|Right Click + Drag|Pan camera|
|Scroll Wheel|Zoom in/out|

### ImGui Panel (if enabled)

- Adjust Lorenz parameters (σ, ρ, β)
- Control simulation speed (steps per frame)
- Modify visualization settings (max points, transparency)
- Real-time FPS monitoring

##  Project Structure

```
lorenz-opengl/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── LICENSE                 # MIT License
├── .gitignore             # Git ignore rules
│
├── include/                # Header files
│   ├── camera.h           # 3D camera system
│   ├── shader.h           # Shader loading/compilation
│   └── lorenz_solver.h    # RK4 integration (header-only)
│
├── src/                    # Implementation files
│   ├── main.cpp           # Application entry point
│   ├── camera.cpp         # Camera implementation
│   └── shader.cpp         # Shader utilities
│
├── shaders/                # GLSL shader programs
│   ├── basic.vert         # Vertex shader
│   └── basic.frag         # Fragment shader
│
├── external/               # Third-party libraries (not in repo)
│   ├── glad/              # OpenGL function loader
│   └── imgui/             # GUI library (optional)
│
└── docs/                   # Documentation assets
    └── screenshot.png     # Demo screenshot
```

##  The (boring || cool) Mathematics 

### Lorenz Equations

The system is governed by three coupled ODEs:

```
dx/dt = σ(y - x)
dy/dt = x(ρ - z) - y
dz/dt = xy - βz
```

**Standard Parameters** (chaotic regime):

- σ = 10 (Prandtl number)
- ρ = 28 (Rayleigh number)
- β = 8/3 (geometric factor)

### Numerical Integration: RK4

We use the 4th-order Runge-Kutta method for high accuracy:

```
k₁ = f(sₙ)
k₂ = f(sₙ + Δt/2 · k₁)
k₃ = f(sₙ + Δt/2 · k₂)
k₄ = f(sₙ + Δt · k₃)

sₙ₊₁ = sₙ + Δt/6 · (k₁ + 2k₂ + 2k₃ + k₄)
```

**Why RK4?**

- Local error: O(Δt⁵)
- Preserves energy in conservative systems
- Critical for chaos: small numerical errors compound exponentially

##  Compiler Flag Warning

**DO NOT use `-ffast-math`** with this code!

Aggressive floating-point optimizations break the numerical precision required for chaos theory. Testing showed:

- **With `-ffast-math`**: 19.4 unit final state error (completely wrong)
- **Without `-ffast-math`**: Accurate attractor structure

See `CMakeLists.txt` for safe optimization flags (`-O3 -march=native`).

## 🔧 Customization

### Modify Lorenz Parameters

Edit `src/main.cpp`:

```cpp
// Line ~38
float sigma = 10.0f;  // Try 10-15
float rho = 28.0f;    // Try 24-35 (bifurcation at 24.74)
float beta = 8.0f / 3.0f;
```

### Change Initial Conditions

Edit `src/main.cpp`:

```cpp
// Line ~113
solver.setState(0.0, 1.0, 0.0);  // (x₀, y₀, z₀)
```

### Adjust Visual Style

Edit `shaders/basic.frag`:

```glsl
// Lines 10-11
vec3 color1 = vec3(0.2, 0.6, 1.0);  // Low altitude color
vec3 color2 = vec3(1.0, 0.3, 0.5);  // High altitude color
```

##  Learning Resources

### Chaos Theory

- **Strogatz, S.H.** (2015). _Nonlinear Dynamics and Chaos_. Westview Press.
- **Lorenz, E.N.** (1963). "Deterministic Nonperiodic Flow." _Journal of the Atmospheric Sciences_, 20(2), 130-141.

### Numerical Methods

- **Press, W.H. et al.** (2007). _Numerical Recipes: The Art of Scientific Computing_. Cambridge University Press.

### OpenGL

- [LearnOpenGL](https://learnopengl.com/) - Comprehensive modern OpenGL tutorial
- [OpenGL Documentation](https://www.khronos.org/opengl/) - Official Khronos specs

##  Troubleshooting

### "Failed to initialize GLAD"

- **Cause**: GLAD not properly installed or OpenGL context creation failed
- **Fix**: Ensure GPU drivers are up to date and GLAD is in `external/glad/`

### Black screen / No rendering

- **Cause**: Shader compilation failure
- **Fix**: Check terminal for shader errors. Ensure shaders are in `build/shaders/`

### Low FPS on Linux

- **Cause**: Software rendering (Mesa llvmpipe)
- **Fix**: Install proprietary GPU drivers (NVIDIA/AMD)

### WSL2 OpenGL version issues

- **Cause**: WSL2 OpenGL-to-D3D12 translation layer limits
- **Fix**: Already configured for OpenGL 4.2 (max supported). Native Linux recommended for 4.6+.

##  Performance Optimization Journey

This project demonstrates systematic optimization:

1. **Python Prototype** (15 FPS): Pure NumPy + Matplotlib
2. **Python + OpenGL** (25 FPS): vispy library
3. **C++ + OpenGL** (116+ FPS): Native implementation
    - Identified bottleneck: 95% GUI rendering, 2% physics
    - Solution: Minimize ImGui overlay, maximize native rendering

**Key Insight**: Rendering dominates computation in real-time visualization systems.

##  Contributing

Contributions welcome! Areas of interest:

- Additional chaotic systems (Rössler, Chen, Halvorsen)
- Advanced shader effects (glow, motion blur, anti-aliasing)
- Poincaré section visualization
- Lyapunov exponent computation
- Recording/export functionality

##  Acknowledgments

- **Edward Lorenz** for discovering the attractor in 1963
- **OpenGL community** for comprehensive documentation
- **GLFW/GLAD/GLM maintainers** for excellent libraries
- **ImGui (Omar Cornut)** for the immediate-mode GUI framework


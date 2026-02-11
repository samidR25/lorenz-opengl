cmake_minimum_required(VERSION 3.15)
project(LorenzOpenGL VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# GLAD (OpenGL loader) - you'll need to add this
# Download from https://glad.dav1d.de/ (OpenGL 4.6 Core)
set(GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad")
if(EXISTS ${GLAD_DIR})
    add_library(glad ${GLAD_DIR}/src/glad.c)
    target_include_directories(glad PUBLIC ${GLAD_DIR}/include)
else()
    message(WARNING "GLAD not found at ${GLAD_DIR}")
    message(WARNING "Please download from https://glad.dav1d.de/")
endif()

# ImGui (optional but recommended)
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
if(EXISTS ${IMGUI_DIR})
    add_library(imgui 
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    )
    target_include_directories(imgui PUBLIC 
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
    )
    target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
    set(HAS_IMGUI TRUE)
else()
    message(WARNING "ImGui not found - building without GUI controls")
    set(HAS_IMGUI FALSE)
endif()

# Main executable
add_executable(lorenz_viz
    src/main.cpp
    src/shader.cpp
    src/camera.cpp
)

target_include_directories(lorenz_viz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(lorenz_viz PRIVATE
    OpenGL::GL
    glfw
    glm::glm
    glad
)

if(HAS_IMGUI)
    target_link_libraries(lorenz_viz PRIVATE imgui)
    target_compile_definitions(lorenz_viz PRIVATE HAS_IMGUI)
endif()

# Optimization flags
target_compile_options(lorenz_viz PRIVATE
    -O3
    -march=native
    -Wall
    -Wextra
)

# Copy shaders to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders 
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Print configuration
message(STATUS "")
message(STATUS "================================================================")
message(STATUS "              Lorenz OpenGL Visualizer Build")
message(STATUS "================================================================")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "OpenGL: ${OPENGL_VERSION}")
message(STATUS "GLFW3: Found")
message(STATUS "GLM: Found")
message(STATUS "GLAD: ${GLAD_DIR}")
if(HAS_IMGUI)
message(STATUS "ImGui: Found (GUI controls enabled)")
else()
message(STATUS "ImGui: Not found (keyboard controls only)")
endif()
message(STATUS "")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "Usage:")
message(STATUS "  make -j4")
message(STATUS "  ./lorenz_viz")
message(STATUS "")
message(STATUS "Controls:")
message(STATUS "  SPACE - Start/Stop simulation")
message(STATUS "  R     - Reset")
message(STATUS "  Mouse - Rotate camera")
message(STATUS "  Scroll - Zoom")
message(STATUS "  ESC   - Exit")
message(STATUS "================================================================")
message(STATUS "")
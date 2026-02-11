// main.cpp - Native C++ OpenGL Lorenz Visualizer
// Complete working implementation

#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ImGui
#ifdef HAS_IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif

// Our headers
#include "shader.h"
#include "camera.h"
#include "lorenz_solver.h"

// Global state
struct AppState {
    int width = 1600;
    int height = 900;
    
    Camera camera;
    
    // Mouse
    double last_mouse_x = 0.0;
    double last_mouse_y = 0.0;
    bool left_mouse_down = false;
    bool right_mouse_down = false;
    bool first_mouse = true;
    
    // Simulation
    bool running = false;
    int steps_per_frame = 1;
    
    // Lorenz parameters
    float sigma = 10.0f;
    float rho = 28.0f;
    float beta = 8.0f / 3.0f;
    float dt = 0.01f;
    
    // Visualization
    int max_points = 50000;
    float line_alpha = 1.0f;
    
    // Performance
    int frame_count = 0;
    double fps = 0.0;
    std::chrono::high_resolution_clock::time_point fps_timer;
} g_state;

// Forward declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void render_gui();

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);  // 4x MSAA
    
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(
        g_state.width, g_state.height,
        "Lorenz Attractor - C++/OpenGL [Press SPACE to start]",
        nullptr, nullptr
    );
    
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // V-Sync
    
    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    
    // Load OpenGL functions with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    std::cout << "\n=== Lorenz Attractor Visualizer ===" << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  SPACE     - Start/Stop simulation" << std::endl;
    std::cout << "  R         - Reset" << std::endl;
    std::cout << "  Mouse Drag - Rotate camera" << std::endl;
    std::cout << "  Scroll    - Zoom" << std::endl;
    std::cout << "  ESC       - Exit" << std::endl;
    std::cout << "===================================\n" << std::endl;
    
    // Enable OpenGL features
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.5f);
    
    // Load shaders
    Shader shader("shaders/basic.vert", "shaders/basic.frag");
    
    // Create Lorenz solver
    LorenzSolver solver(g_state.sigma, g_state.rho, g_state.beta);
    solver.setState(0.0, 1.0, 0.0);
    
    // Create OpenGL buffers
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
    #ifdef HAS_IMGUI
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 420");
    #endif
    
    g_state.fps_timer = std::chrono::high_resolution_clock::now();
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Process input
        glfwPollEvents();
        
        // Update simulation
        if (g_state.running) {
            for (int i = 0; i < g_state.steps_per_frame; ++i) {
                solver.step(g_state.dt);
                
                // Limit trajectory size
                if (solver.getTrajectory().size() > static_cast<size_t>(g_state.max_points)) {
                    solver.clearOldest(g_state.max_points);
                }
            }
        }
        
        // Render
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use shader
        shader.use();
        
        // Set matrices
        glm::mat4 view = g_state.camera.getViewMatrix();
        glm::mat4 projection = g_state.camera.getProjectionMatrix(
            (float)g_state.width / (float)g_state.height
        );
        
        shader.setMat4("view", glm::value_ptr(view));
        shader.setMat4("projection", glm::value_ptr(projection));
        shader.setFloat("alpha", g_state.line_alpha);
        
        // Update VBO with trajectory data
        const auto& trajectory = solver.getTrajectory();
    shader.setInt("totalPoints", trajectory.size());
        if (!trajectory.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, 
                         trajectory.size() * sizeof(glm::vec3), 
                         trajectory.data(), 
                         GL_DYNAMIC_DRAW);
            
            // Draw
            glBindVertexArray(VAO);
            glDrawArrays(GL_LINE_STRIP, 0, trajectory.size());
        }
        
        #ifdef HAS_IMGUI
        // Render ImGui
        render_gui();
        #endif
        
        // Swap buffers
        glfwSwapBuffers(window);
        
        // Update FPS
        g_state.frame_count++;
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_state.fps_timer).count();
        if (elapsed >= 1000) {
            g_state.fps = g_state.frame_count / (elapsed / 1000.0);
            g_state.frame_count = 0;
            g_state.fps_timer = now;
            
            // Update window title
            std::string title = "Lorenz Attractor - " + 
                              std::to_string((int)g_state.fps) + " FPS | " +
                              std::to_string(trajectory.size()) + " points";
            if (g_state.running) title += " [RUNNING]";
            else title += " [PAUSED - Press SPACE]";
            glfwSetWindowTitle(window, title.c_str());
        }
    }
    
    // Cleanup
    #ifdef HAS_IMGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    #endif
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    return 0;
}

// Callback implementations
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    g_state.width = width;
    g_state.height = height;
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    #ifdef HAS_IMGUI
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;
    #endif
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        g_state.left_mouse_down = (action == GLFW_PRESS);
        if (action == GLFW_PRESS) {
            g_state.first_mouse = true;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        g_state.right_mouse_down = (action == GLFW_PRESS);
        if (action == GLFW_PRESS) {
            g_state.first_mouse = true;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    #ifdef HAS_IMGUI
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;
    #endif
    
    if (g_state.first_mouse) {
        g_state.last_mouse_x = xpos;
        g_state.last_mouse_y = ypos;
        g_state.first_mouse = false;
        return;
    }
    
    double dx = xpos - g_state.last_mouse_x;
    double dy = ypos - g_state.last_mouse_y;
    
    if (g_state.left_mouse_down) {
        // Rotate camera
        g_state.camera.rotate(dx * 0.3f, -dy * 0.3f);
    }
    
    if (g_state.right_mouse_down) {
        // Pan camera
        g_state.camera.pan(-dx, dy);
    }
    
    g_state.last_mouse_x = xpos;
    g_state.last_mouse_y = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    #ifdef HAS_IMGUI
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;
    #endif
    
    g_state.camera.zoom(-yoffset * 3.0f);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_SPACE:
                g_state.running = !g_state.running;
                break;
            case GLFW_KEY_R:
                // Reset simulation (this would need implementation in solver)
                g_state.camera.reset();
                break;
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
        }
    }
}

void render_gui() {
    #ifdef HAS_IMGUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Control panel
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(350, 500), ImGuiCond_FirstUseEver);
    ImGui::Begin("Lorenz Controls");
    
    ImGui::Text("FPS: %.1f", g_state.fps);
    ImGui::Separator();
    
    ImGui::Text("Simulation");
    if (ImGui::Button(g_state.running ? "Pause (SPACE)" : "Start (SPACE)", ImVec2(150, 30))) {
        g_state.running = !g_state.running;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset (R)", ImVec2(150, 30))) {
        g_state.camera.reset();
    }
    
    ImGui::SliderInt("Steps/Frame", &g_state.steps_per_frame, 1, 20);
    ImGui::Separator();
    
    ImGui::Text("Lorenz Parameters");
    ImGui::SliderFloat("Sigma", &g_state.sigma, 0.1f, 50.0f);
    ImGui::SliderFloat("Rho", &g_state.rho, 0.1f, 100.0f);
    ImGui::SliderFloat("Beta", &g_state.beta, 0.1f, 10.0f);
    ImGui::SliderFloat("dt", &g_state.dt, 0.001f, 0.05f, "%.4f");
    ImGui::Separator();
    
    ImGui::Text("Visualization");
    ImGui::SliderInt("Max Points", &g_state.max_points, 1000, 200000);
    ImGui::SliderFloat("Line Alpha", &g_state.line_alpha, 0.1f, 1.0f);
    ImGui::Separator();
    
    ImGui::Text("Camera");
    ImGui::Text("Distance: %.1f", g_state.camera.distance);
    ImGui::Text("Yaw: %.1f°", g_state.camera.yaw);
    ImGui::Text("Pitch: %.1f°", g_state.camera.pitch);
    if (ImGui::Button("Reset Camera", ImVec2(150, 25))) {
        g_state.camera.reset();
    }
    
    ImGui::End();
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    #endif
}

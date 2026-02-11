// basic.vert - Vertex Shader for Lorenz Attractor
#version 420 core

layout(location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;
uniform float pointIndex;  // For color gradient
uniform float totalPoints;

out vec3 fragColor;

void main() {
    // Transform position
    gl_Position = projection * view * vec4(aPos, 1.0);
    
    // Color gradient based on position in trajectory
    // Blue (start) → Cyan → Green → Yellow → Red (end)
    float t = pointIndex / totalPoints;
    
    if (t < 0.25) {
        // Blue to Cyan
        float local_t = t / 0.25;
        fragColor = vec3(0.0, local_t, 1.0);
    } else if (t < 0.5) {
        // Cyan to Green
        float local_t = (t - 0.25) / 0.25;
        fragColor = vec3(0.0, 1.0, 1.0 - local_t);
    } else if (t < 0.75) {
        // Green to Yellow
        float local_t = (t - 0.5) / 0.25;
        fragColor = vec3(local_t, 1.0, 0.0);
    } else {
        // Yellow to Red
        float local_t = (t - 0.75) / 0.25;
        fragColor = vec3(1.0, 1.0 - local_t, 0.0);
    }
}

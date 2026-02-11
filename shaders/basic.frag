// basic.frag - Fragment Shader for Lorenz Attractor
#version 420 core

in vec3 fragColor;
out vec4 FragColor;

uniform float alpha;  // Transparency

void main() {
    FragColor = vec4(fragColor, alpha);
}

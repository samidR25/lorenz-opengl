// camera.h - 3D camera control for Lorenz visualization
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    // Camera position and orientation
    float distance;
    float yaw;
    float pitch;
    glm::vec3 target;
    
    // Camera settings
    float fov;
    float near_plane;
    float far_plane;
    
    Camera(float dist = 60.0f, float yaw_deg = 45.0f, float pitch_deg = 20.0f);
    
    // Get view matrix
    glm::mat4 getViewMatrix() const;
    
    // Get projection matrix
    glm::mat4 getProjectionMatrix(float aspect_ratio) const;
    
    // Get camera position in world space
    glm::vec3 getPosition() const;
    
    // Camera controls
    void rotate(float delta_yaw, float delta_pitch);
    void zoom(float delta);
    void pan(float delta_x, float delta_y);
    
    // Reset camera to default
    void reset();

private:
    // Default values (for reset)
    float default_distance;
    float default_yaw;
    float default_pitch;
    glm::vec3 default_target;
};

#endif
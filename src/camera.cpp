// camera.cpp - 3D camera control implementation
#include "camera.h"
#include <algorithm>

Camera::Camera(float dist, float yaw_deg, float pitch_deg)
    : distance(dist)
    , yaw(yaw_deg)
    , pitch(pitch_deg)
    , target(0.0f, 0.0f, 25.0f)
    , fov(45.0f)
    , near_plane(0.1f)
    , far_plane(1000.0f)
    , default_distance(dist)
    , default_yaw(yaw_deg)
    , default_pitch(pitch_deg)
    , default_target(0.0f, 0.0f, 25.0f)
{
}

glm::mat4 Camera::getViewMatrix() const {
    glm::vec3 camera_pos = getPosition();
    glm::vec3 up(0.0f, 0.0f, 1.0f);  // Z-up coordinate system
    return glm::lookAt(camera_pos, target, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspect_ratio) const {
    return glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
}

glm::vec3 Camera::getPosition() const {
    // Convert spherical coordinates to Cartesian
    float yaw_rad = glm::radians(yaw);
    float pitch_rad = glm::radians(pitch);
    
    glm::vec3 offset;
    offset.x = distance * cos(pitch_rad) * cos(yaw_rad);
    offset.y = distance * cos(pitch_rad) * sin(yaw_rad);
    offset.z = distance * sin(pitch_rad);
    
    return target + offset;
}

void Camera::rotate(float delta_yaw, float delta_pitch) {
    yaw += delta_yaw;
    pitch += delta_pitch;
    
    // Clamp pitch to avoid gimbal lock
    pitch = std::clamp(pitch, -89.0f, 89.0f);
    
    // Wrap yaw
    if (yaw > 360.0f) yaw -= 360.0f;
    if (yaw < 0.0f) yaw += 360.0f;
}

void Camera::zoom(float delta) {
    distance += delta;
    distance = std::clamp(distance, 5.0f, 200.0f);
}

void Camera::pan(float delta_x, float delta_y) {
    // Get right and up vectors
    glm::vec3 forward = glm::normalize(target - getPosition());
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 0, 1)));
    glm::vec3 up = glm::cross(right, forward);
    
    // Pan is scaled by distance
    float pan_speed = distance * 0.01f;
    target += right * delta_x * pan_speed;
    target += up * delta_y * pan_speed;
}

void Camera::reset() {
    distance = default_distance;
    yaw = default_yaw;
    pitch = default_pitch;
    target = default_target;
}
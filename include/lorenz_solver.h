// lorenz_solver.h - GPU-friendly Lorenz solver
#ifndef LORENZ_SOLVER_H
#define LORENZ_SOLVER_H

#include <vector>
#include <glm/glm.hpp>

class LorenzSolver {
public:
    LorenzSolver(float sigma = 10.0f, float rho = 28.0f, float beta = 8.0f/3.0f)
        : sigma_(sigma), rho_(rho), beta_(beta) {
        trajectory_.reserve(50000);
        trajectory_.push_back(state_);
    }
    
    void setParameters(float sigma, float rho, float beta) {
        sigma_ = sigma;
        rho_ = rho;
        beta_ = beta;
    }
    
    void setState(float x, float y, float z) {
        state_ = glm::vec3(x, y, z);
        trajectory_.clear();
        trajectory_.push_back(state_);
    }
    
    void step(float dt) {
        // RK4 integration
        glm::vec3 k1 = derivatives(state_);
        glm::vec3 k2 = derivatives(state_ + 0.5f * dt * k1);
        glm::vec3 k3 = derivatives(state_ + 0.5f * dt * k2);
        glm::vec3 k4 = derivatives(state_ + dt * k3);
        
        state_ += (dt / 6.0f) * (k1 + 2.0f*k2 + 2.0f*k3 + k4);
        trajectory_.push_back(state_);
    }
    
    const std::vector<glm::vec3>& getTrajectory() const {
        return trajectory_;
    }
    
    glm::vec3 getState() const {
        return state_;
    }
    
    void clearOldest(size_t keep) {
        if (trajectory_.size() > keep) {
            trajectory_.erase(trajectory_.begin(), 
                            trajectory_.begin() + (trajectory_.size() - keep));
        }
    }
    
    void reset() {
        trajectory_.clear();
        state_ = glm::vec3(0.0f, 1.0f, 0.0f);
        trajectory_.push_back(state_);
    }

private:
    glm::vec3 derivatives(const glm::vec3& state) const {
        return glm::vec3(
            sigma_ * (state.y - state.x),
            state.x * (rho_ - state.z) - state.y,
            state.x * state.y - beta_ * state.z
        );
    }
    
    float sigma_, rho_, beta_;
    glm::vec3 state_{0.0f, 1.0f, 0.0f};
    std::vector<glm::vec3> trajectory_;
};

#endif // LORENZ_SOLVER_H
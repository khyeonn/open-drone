#pragma once

#include <math/Eigen/Core>

struct DroneState {
    Eigen::Vector3d velocity;
    Eigen::Vector3d angular_velocity;
    Eigen::Vector3d euler_angle;
    Eigen::Vector3d position;
};
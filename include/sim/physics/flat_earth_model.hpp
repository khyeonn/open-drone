#pragma once

// #include "sim/types/drone_state.hpp"
#include <math/Eigen/Core>

class FlatEarthEOM {
public:
    FlatEarthEOM(Eigen::Matrix<double, 12, 1> _state);

private:
    Eigen::Matrix<double, 12, 1> state;
};
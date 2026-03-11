#pragma once

// #include "sim/types/drone_state.hpp"
#include "sim/types/aux_data.hpp"
#include <math/Eigen/Core>

namespace physics {

class FlatEarthEOM {
public:
    Eigen::Matrix<double, 12, 1>
    derivative(Eigen::Matrix<double, 12, 1> const& x,
               sim::AuxData* aux = nullptr) const;
};

} // namespace physics

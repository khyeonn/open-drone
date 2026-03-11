#pragma once

#include "sim/physics/flat_earth_model.hpp"
#include "sim/types/aux_data.hpp"
#include <sim/math/Eigen/Core>
#include <vector>

namespace numerical_methods {

inline void forward_euler(physics::FlatEarthEOM const& model,
                          Eigen::Matrix<double, 12, Eigen::Dynamic>& x,
                          std::vector<double> const& t_s, double h_s,
                          std::vector<sim::AuxData>* aux_log = nullptr) {
    std::size_t const nt_s = t_s.size();

    sim::AuxData aux;

    for (std::size_t i = 1; i < nt_s; ++i) {
        x.col(i) = x.col(i - 1) + h_s * model.derivative(x.col(i - 1), &aux);
        aux_log->push_back(aux);
    }
}

} // namespace numerical_methods
#pragma once

#include "sim/math/numerical_methods/rk4_step.hpp"
#include "sim/types/aux_data.hpp"
#include "sim/types/drone_state.hpp"
#include <sim/math/Eigen/Core>
#include <vector>

namespace sim::numerical_methods {

template <typename Model>
inline void rk4(Model const& model,
                Eigen::Matrix<double, 12, Eigen::Dynamic>& x_hist,
                std::vector<double> const& t_s, double h_s,
                std::vector<types::AuxData>* aux_log = nullptr) {
    std::size_t const nt_s = t_s.size();

    types::State x = x_hist.col(0);

    for (std::size_t i = 1; i < nt_s; ++i) {
        types::AuxData aux{};
        rk4_step(model, x, h_s, aux_log ? &aux : nullptr);

        x_hist.col(i) = x;

        if (aux_log != nullptr) {
            aux_log->push_back(aux);
        }
    }
}

} // namespace sim::numerical_methods
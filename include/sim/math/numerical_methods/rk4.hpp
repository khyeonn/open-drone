#pragma once

#include "sim/types/aux_data.hpp"
#include <sim/math/Eigen/Core>
#include <vector>

namespace sim::numerical_methods {

template <typename Model>
inline void rk4(Model const& model,
                Eigen::Matrix<double, 12, Eigen::Dynamic>& x,
                std::vector<double> const& t_s, double h_s,
                std::vector<types::AuxData>* aux_log = nullptr) {
    std::size_t const nt_s = t_s.size();

    types::AuxData aux;

    for (std::size_t i = 1; i < nt_s; ++i) {
        auto f1 = model.derivative(x.col(i - 1), nullptr);
        auto k1 = h_s * f1;
        auto f2 = model.derivative(x.col(i - 1) + 0.5 * k1, nullptr);
        auto k2 = h_s * f2;
        auto f3 = model.derivative(x.col(i - 1) + 0.5 * k2, nullptr);
        auto k3 = h_s * f3;
        auto f4 = model.derivative(x.col(i - 1) + k3, nullptr);
        auto k4 = h_s * f4;

        x.col(i) = x.col(i - 1) + (k1 + 2.0 * k2 + 2.0 * k3 + k4) / 6;

        if (aux_log != nullptr) {
            types::AuxData aux{};
            (void)model.derivative(x.col(i), &aux);
            aux_log->push_back(aux);
        }
    }
}

} // namespace sim::numerical_methods
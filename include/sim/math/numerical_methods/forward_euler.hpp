#pragma once

#include "sim/types/aux_data.hpp"
#include <sim/math/Eigen/Core>
#include <vector>

namespace sim::numerical_methods {

template <typename Model>
inline void forward_euler(Model const& model,
                          Eigen::Matrix<double, 12, Eigen::Dynamic>& x,
                          std::vector<double> const& t_s, double h_s,
                          std::vector<types::AuxData>* aux_log = nullptr) {
    std::size_t const nt_s = t_s.size();

    for (std::size_t i = 1; i < nt_s; ++i) {
        types::AuxData aux{};
        x.col(i) = x.col(i - 1) + h_s * model.derivative(x.col(i - 1), &aux);

        if (aux_log != nullptr)
            aux_log->push_back(aux);
    }
}

} // namespace sim::numerical_methods
#pragma once

#include "sim/types/aux_data.hpp"
#include "sim/types/drone_state.hpp"

namespace sim::numerical_methods {

template <typename Model>
inline void rk4_step(Model const& model, types::State& x, double h_s,
                     types::AuxData* aux = nullptr) {
    types::State const k1 = h_s * model.derivative(x, nullptr);
    types::State const k2 = h_s * model.derivative(x + 0.5 * k1, nullptr);
    types::State const k3 = h_s * model.derivative(x + 0.5 * k2, nullptr);
    types::State const k4 = h_s * model.derivative(x + k3, nullptr);

    x += (k1 + 2.0 * k2 + 2.0 * k3 + k4) / 6.0;

    if (aux != nullptr) {
        (void)model.derivative(x, aux);
    }
}

} // namespace sim::numerical_methods
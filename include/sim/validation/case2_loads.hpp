#pragma once

#include "sim/types/aux_data.hpp"
#include "sim/types/drone_state.hpp"
#include "sim/types/external_loads.hpp"
#include "sim/types/flight_conditions.hpp"
#include <cmath>

namespace sim::loads {

template <typename Vehicle> struct Case2Loads {
    types::ExternalLoads compute(Vehicle const& vehicle, types::State const& x,
                                 types::FlightCondition const& fc,
                                 types::AuxData* aux = nullptr) const {
        types::ExternalLoads loads{};

        double const alpha_rad = fc.alpha_rad;
        double const beta_rad  = fc.beta_rad;
        double const qbar      = fc.dynamic_pressure;
        double const Aref      = vehicle.A_ref_m2;

        double const c_alpha = std::cos(alpha_rad);
        double const s_alpha = std::sin(alpha_rad);
        double const c_beta  = std::cos(beta_rad);
        double const s_beta  = std::sin(beta_rad);

        double C_l =
            vehicle.Clp * x(3) * vehicle.b_m / (2 * fc.true_airspeed_mps)
            + vehicle.Clr * x(5) * vehicle.b_m / (2 * fc.true_airspeed_mps);

        double C_m =
            vehicle.Cmq * x(4) * vehicle.c_m / (2 * fc.true_airspeed_mps);

        double C_n =
            vehicle.Cnp * x(3) * vehicle.b_m / (2 * fc.true_airspeed_mps)
            + vehicle.Cnr * x(5) * vehicle.b_m / (2 * fc.true_airspeed_mps);

        loads.fx_b_N = 0.0;
        loads.fy_b_N = 0.0;
        loads.fz_b_N = 0.0;

        loads.mx_b_Nm = C_l * qbar * Aref * vehicle.b_m;
        loads.my_b_Nm = C_m * qbar * Aref * vehicle.c_m;
        loads.mz_b_Nm = C_n * qbar * Aref * vehicle.b_m;

        return loads;
    }
};

} // namespace sim::loads
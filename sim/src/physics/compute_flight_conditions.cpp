#include "sim/physics/compute_flight_conditions.hpp"
#include "sim/physics/ussa1976.hpp"
#include <cmath>

namespace sim::physics {

types::FlightCondition compute_flight_condition(types::State const& x) {
    types::FlightCondition fc{};

    double const vx_mps     = x(0);
    double const vy_mps     = x(1);
    double const vz_mps     = x(2);
    double const altitude_m = -x(11);

    double const v_true =
        std::sqrt(vx_mps * vx_mps + vy_mps * vy_mps + vz_mps * vz_mps);

    fc.true_airspeed_mps  = v_true;
    fc.air_density_kgpm3  = physics::ussa1976_density(altitude_m);
    fc.speed_of_sound_mps = physics::ussa1976_v_sound(altitude_m);
    fc.dynamic_pressure   = 0.5 * fc.air_density_kgpm3 * v_true * v_true;
    fc.alpha_rad          = std::atan2(vz_mps, vx_mps + 1E-9);
    fc.beta_rad           = std::asin(vy_mps / v_true + 1E-9);
    fc.mach               = fc.true_airspeed_mps / fc.speed_of_sound_mps;

    return fc;
}

} // namespace sim::physics
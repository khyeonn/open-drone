#pragma once

namespace sim::types {

struct FlightCondition {
    double air_density_kgpm3  = 0.0;
    double true_airspeed_mps  = 0.0;
    double dynamic_pressure   = 0.0;
    double speed_of_sound_mps = 0.0;
    double mach               = 0.0;
    double alpha_rad          = 0.0;
    double beta_rad           = 0.0;
    double omega_x            = 0.0;
    double omega_y            = 0.0;
    double omega_z            = 0.0;
};

} // namespace sim::types
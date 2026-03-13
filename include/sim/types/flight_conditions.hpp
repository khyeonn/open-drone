#pragma once

namespace sim::types {

struct FlightCondition {
    double air_density_kgpm3{};
    double true_airspeed_mps{};
    double dynamic_pressure{};
    double speed_of_sound_mps{};
    double mach{};
    double alpha_rad{};
    double beta_rad{};
    double omega_x{};
    double omega_y{};
    double omega_z{};
};

} // namespace sim::types
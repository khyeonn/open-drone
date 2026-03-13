#pragma once

#include <cmath>

namespace sim::vehicles {

struct SphereVehicle {
    static constexpr double in2m        = 0.0254;
    static constexpr double rho_kgpm3   = 7868.36;
    static constexpr double r_sphere_in = 3.0;
    static constexpr double r_sphere_m  = r_sphere_in * in2m;

    static constexpr double vol_sphere_m3 =
        4.0 / 3.0 * M_PI * r_sphere_m * r_sphere_m * r_sphere_m;
    static constexpr double m_kg     = rho_kgpm3 * vol_sphere_m3;
    static constexpr double J_kgm2   = 0.4 * m_kg * r_sphere_m * r_sphere_m;
    static constexpr double A_ref_m2 = M_PI * r_sphere_m * r_sphere_m;

    static constexpr double Jxx_kgm2 = J_kgm2;
    static constexpr double Jyy_kgm2 = J_kgm2;
    static constexpr double Jzz_kgm2 = J_kgm2;
    static constexpr double Jxz_kgm2 = 0.0;

    static constexpr double b_m = r_sphere_m; // Reference wing span
    static constexpr double c_m = r_sphere_m; // Reference wing chord

    static constexpr double Clp = 0.0; // Roll damping from roll rate
    static constexpr double Clr = 0.0; // Roll damping from yaw rate
    static constexpr double Cmq = 0.0; // Pitch damping from pitch rate
    static constexpr double Cnp = 0.0; // Yaw damping from roll rate
    static constexpr double Cnr = 0.0; // Yaw damping from yaw rate
};

} // namespace sim::vehicles
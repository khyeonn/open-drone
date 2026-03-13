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
};

} // namespace sim::vehicles
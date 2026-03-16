#pragma once

#include <cmath>

namespace sim::vehicles {

struct BrickVehicleDamped {
    static constexpr double in2m    = 0.0254;
    static constexpr double slug2kg = 14.5939;
    static constexpr double kg2slug = 1 / slug2kg;
    static constexpr double ft2m    = 0.304878;

    static constexpr double m_brick_slug = 0.1554048;
    static constexpr double m_kg         = kg2slug * m_brick_slug;

    static constexpr double Jxx_slugft2 = 0.00189422;
    static constexpr double Jxx_kgm2    = slug2kg * (ft2m * ft2m) * Jxx_slugft2;

    static constexpr double Jyy_slugft2 = 0.00621102;
    static constexpr double Jyy_kgm2    = slug2kg * (ft2m * ft2m) * Jyy_slugft2;

    static constexpr double Jzz_slugft2 = 0.00719467;
    static constexpr double Jzz_kgm2    = slug2kg * (ft2m * ft2m) * Jzz_slugft2;

    static constexpr double Jxz_slugft2 = 0;
    static constexpr double Jxz_kgm2    = slug2kg * (ft2m * ft2m) * Jxz_slugft2;

    static constexpr double length_brick_m = 8 * in2m;
    static constexpr double width_brick_m  = 4 * in2m;
    static constexpr double A_ref_m2       = length_brick_m * width_brick_m;

    static constexpr double b_m = 0.33333 * ft2m; // Reference wing span
    static constexpr double c_m = 0.66667 * ft2m; // Reference wing chord

    static constexpr double Clp = -1.0; // Roll damping from roll rate
    static constexpr double Clr = 0.0;  // Roll damping from yaw rate
    static constexpr double Cmq = -1.0; // Pitch damping from pitch rate
    static constexpr double Cnp = 0.0;  // Yaw damping from roll rate
    static constexpr double Cnr = -1.0; // Yaw damping from yaw rate
};

} // namespace sim::vehicles
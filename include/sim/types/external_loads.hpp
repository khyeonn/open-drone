#pragma once

namespace sim::types {

struct ExternalLoads {
    double fx_b_N{};
    double fy_b_N{};
    double fz_b_N{};

    double mx_b_Nm{};
    double my_b_Nm{};
    double mz_b_Nm{};
};

} // namespace sim::types
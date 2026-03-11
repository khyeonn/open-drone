#include "core/utils/arange.hpp"
#include "core/utils/export_to_csv.hpp"
#include "sim/math/numerical_methods/rk4.hpp"
#include <cmath>
#include <cstdio>

#include <math/Eigen/Core>
#include <vector>

int main() {
    std::printf("Running simulation for NESC Atmospheric Case 01...\n");
    // Initial conditions
    double u0_bf_mps  = 1E-10;
    double v0_bf_mps  = 0;
    double w0_bf_mps  = 0;
    double p0_bf_rps  = 0;
    double q0_bf_rps  = 0;
    double r0_bf_rps  = 0;
    double phi0_rad   = 0 * M_PI / 180;
    double theta0_rad = -90 * M_PI / 180;
    double psi0_rad   = 0 * M_PI / 180;
    double p10_n_m    = 0;
    double p20_n_m    = 0;
    double p30_n_m    = -30000 / 3.28;

    // Simulation settings
    double t0_s = 0.0;  // Start time
    double tf_s = 30;   // End time
    double h_s  = 0.01; // Step size [s]

    // State vector
    Eigen::Matrix<double, 12, 1> x0 =
        (Eigen::Matrix<double, 12, 1>() << u0_bf_mps, v0_bf_mps, w0_bf_mps,
         p0_bf_rps, q0_bf_rps, r0_bf_rps, phi0_rad, theta0_rad, psi0_rad,
         p10_n_m, p20_n_m, p30_n_m)
            .finished();

    // Time vector
    std::vector<double> t_s = utils::arange(t0_s, tf_s + h_s, h_s);
    std::size_t nt_s        = t_s.size();

    Eigen::Matrix<double, 12, Eigen::Dynamic> x;
    x.resize(12, nt_s);
    x.col(0) = x0;

    physics::FlatEarthEOM model;
    std::vector<sim::AuxData> aux_log{};

    // Numerical approximation
    numerical_methods::rk4(model, x, t_s, h_s, &aux_log);
    utils::export_to_csv(x, t_s, aux_log, "sim_data_01.csv");

    std::printf("Simulation complete. Results saved\n");

    return 0;
}
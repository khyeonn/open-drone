#include "sim/math/numerical_methods/rk4.hpp"
#include "sim/physics/rigid_body_6dof_model.hpp"
#include "sim/utils/arange.hpp"
#include "sim/utils/export_to_csv.hpp"
#include "sim/validation/case2_loads.hpp"
#include "sim/vehicle_models/brick_vehicle.hpp"

#include <cmath>
#include <cstdio>
#include <math/Eigen/Core>
#include <vector>

int main() {
    std::printf("Running NESC Atmospheric Check Case 2...\n");

    constexpr double deg2rad = M_PI / 180;

    // Initial conditions
    double u0_bf_mps  = 1E-10;
    double v0_bf_mps  = 0;
    double w0_bf_mps  = 0;
    double p0_bf_rps  = 10.0 * deg2rad;
    double q0_bf_rps  = 20.0 * deg2rad;
    double r0_bf_rps  = 30.0 * deg2rad;
    double phi0_rad   = 0 * deg2rad;
    double theta0_rad = 0 * deg2rad;
    double psi0_rad   = 0 * deg2rad;
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
    std::vector<double> t_s = sim::utils::arange(t0_s, tf_s + h_s, h_s);
    std::size_t nt_s        = t_s.size();

    Eigen::Matrix<double, 12, Eigen::Dynamic> x;
    x.resize(12, nt_s);
    x.col(0) = x0;

    using Vehicle = sim::vehicles::BrickVehicle;
    using Loads   = sim::loads::Case2Loads<Vehicle>;

    sim::physics::Model<Vehicle, Loads> model{};
    std::vector<sim::types::AuxData> aux_log{};

    // Numerical approximation
    std::filesystem::path out_dir =
        "./sim/validation/nesc_cases/case_02/sim_results_csv";
    sim::numerical_methods::rk4(model, x, t_s, h_s, &aux_log);
    sim::utils::export_to_csv(x, t_s, aux_log, out_dir, "sim_data_02.csv");

    std::printf("Simulation complete. Results saved\n");

    return 0;
}
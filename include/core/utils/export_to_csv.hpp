#pragma once

#include "sim/types/aux_data.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sim/math/Eigen/Core>

namespace utils {

inline std::filesystem::path const CSV_OUTPUT_DIR =
    "./sim/validation/nesc_cases/case_01/sim_results_csv";

inline void export_to_csv(Eigen::Matrix<double, 12, Eigen::Dynamic> const& x,
                          std::vector<double> const& t_s,
                          std::vector<sim::AuxData> const& aux,
                          std::string const& filename) {
    std::filesystem::create_directories(CSV_OUTPUT_DIR);

    std::filesystem::path filepath = CSV_OUTPUT_DIR / filename;
    std::ofstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    size_t const nt_s = t_s.size();

    int const aux_rows = 29;
    int total_rows     = x.rows() + aux_rows;

    // Write header
    file << "time";
    file << ",x_vel_mps" << ",y_vel_mps" << ",z_vel_mps" << ",roll_vel_rps"
         << ",pitch_vel_rps" << ",yaw_vel_rps" << ",roll_angle"
         << ",pitch_angle" << ",yaw_angle" << ",x_pos_ned" << ",y_pos_ned"
         << ",z_pos_ned";
    file << ",altitude_m" << ",speed_of_sound_mps" << ",air_density" << ",mach"
         << ",alpha_rad" << ",beta_rad" << ",true_airspeed_mps" << ",c_phi"
         << ",c_theta" << ",c_psi" << ",s_phi" << ",s_theta" << ",s_psi"
         << ",t_theta" << ",dcm_b2i_11" << ",dcm_b2i_12" << ",dcm_b2i_13"
         << ",dcm_b2i_21" << ",dcm_b2i_22" << ",dcm_b2i_23" << ",dcm_b2i_31"
         << ",dcm_b2i_32" << ",dcm_b2i_33" << ",vx_ned_mps" << ",vy_ned_mps"
         << ",vz_ned_mps" << ",roll_dcm_rad" << ",pitch_dcm_rad"
         << ",yaw_dcm_rad" << "\n";

    for (size_t k = 0; k < nt_s; ++k) {
        file << t_s[k];

        // state at timestep k
        for (int i = 0; i < x.rows(); ++i) {
            file << "," << x(i, k);
        }

        // aux data at timestep k
        sim::AuxData const& a = aux[k];
        file << "," << a.altitude_m;
        file << "," << a.speed_of_sound_mps;
        file << "," << a.rho_kgpm3;
        file << "," << a.mach;
        file << "," << a.alpha_rad;
        file << "," << a.beta_rad;
        file << "," << a.true_airspeed_mps;
        file << "," << a.c_phi;
        file << "," << a.c_theta;
        file << "," << a.c_psi;
        file << "," << a.s_phi;
        file << "," << a.s_theta;
        file << "," << a.s_psi;
        file << "," << a.t_theta;
        file << "," << a.dcm_body2inertial_11;
        file << "," << a.dcm_body2inertial_12;
        file << "," << a.dcm_body2inertial_13;
        file << "," << a.dcm_body2inertial_21;
        file << "," << a.dcm_body2inertial_22;
        file << "," << a.dcm_body2inertial_23;
        file << "," << a.dcm_body2inertial_31;
        file << "," << a.dcm_body2inertial_32;
        file << "," << a.dcm_body2inertial_33;
        file << "," << a.vx_ned_mps;
        file << "," << a.vy_ned_mps;
        file << "," << a.vz_ned_mps;
        file << "," << a.roll_from_dcm_rad;
        file << "," << a.pitch_from_dcm_rad;
        file << "," << a.yaw_from_dcm_rad;

        file << "\n";
    }

    file.close();
}
} // namespace utils
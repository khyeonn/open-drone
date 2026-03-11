#pragma once

namespace sim {

// Total number of variables = 29
struct AuxData {
    double altitude_m;           // x0
    double speed_of_sound_mps;   // x1
    double rho_kgpm3;            // x2
    double mach;                 // x3
    double alpha_rad;            // x4
    double beta_rad;             // x5
    double true_airspeed_mps;    // x6
    double c_phi;                // x7
    double c_theta;              // x8
    double c_psi;                // x9
    double s_phi;                // x10
    double s_theta;              // x11
    double s_psi;                // x12
    double t_theta;              // x13
    double dcm_body2inertial_11; // x14
    double dcm_body2inertial_12; // x15
    double dcm_body2inertial_13; // x16
    double dcm_body2inertial_21; // x17
    double dcm_body2inertial_22; // x18
    double dcm_body2inertial_23; // x19
    double dcm_body2inertial_31; // x20
    double dcm_body2inertial_32; // x21
    double dcm_body2inertial_33; // x22
    double vx_ned_mps;           // x23
    double vy_ned_mps;           // x24
    double vz_ned_mps;           // x25
    double roll_from_dcm_rad;    // x26
    double pitch_from_dcm_rad;   // x27
    double yaw_from_dcm_rad;     // x28
};

} // namespace sim

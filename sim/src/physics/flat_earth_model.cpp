#include "sim/physics/flat_earth_model.hpp"
#include "sim/physics/ussa1976.hpp"

Eigen::Matrix<double, 12, 1>
physics::FlatEarthEOM::derivative(Eigen::Matrix<double, 12, 1> const& x,
                                  sim::AuxData* aux) const {
    Eigen::Matrix<double, 12, 1> dx;
    dx.setZero();

    double vx_body_mps = x(0);
    double vy_body_mps = x(1);
    double vz_body_mps = x(2);
    double wx_body_rps = x(3);
    double wy_body_rps = x(4);
    double wz_body_rps = x(5);
    double phi_rad     = x(6);
    double theta_rad   = x(7);
    double psi_rad     = x(8);
    double xpos_ned_m  = x(9);
    double ypos_ned_m  = x(10);
    double zpos_ned_m  = x(11);

    double c_phi   = std::cos(phi_rad);
    double c_theta = std::cos(theta_rad);
    double c_psi   = std::cos(psi_rad);
    double s_phi   = std::sin(phi_rad);
    double s_theta = std::sin(theta_rad);
    double s_psi   = std::sin(psi_rad);
    double t_theta = std::tan(theta_rad);

    // TODO: Create vehicle specifications struct to store different vehicle
    // configurations Hard coded for now
    double in2m = 0.0254;

    double rho_lead_kgpm3 = 7868.36;
    double r_sphere_in    = 3;
    double r_sphere_m     = r_sphere_in * in2m;
    double CD_approx      = 0.5;

    double vol_sphere_m3 =
        4.0 / 3.0 * M_PI * r_sphere_m * r_sphere_m * r_sphere_m;
    double m_sphere_kg   = rho_lead_kgpm3 * vol_sphere_m3;
    double J_sphere_kgm2 = 0.4 * m_sphere_kg * r_sphere_m * r_sphere_m;
    double Aref_m2       = M_PI * r_sphere_m * r_sphere_m;

    double m_kg          = rho_lead_kgpm3 * vol_sphere_m3;
    double Jxz_body_kgm2 = 0;
    double Jxx_body_kgm2 = J_sphere_kgm2;
    double Jyy_b_kgm2    = J_sphere_kgm2;
    double Jzz_body_kgm2 = J_sphere_kgm2;
    // END TODO

    // current altitude
    double h_m = -zpos_ned_m;

    // physics::AtmosphereRowData atm = physics::ussa1976_lookup(h_m);

    double rho_interp_kgpm3 = physics::ussa1976_density(h_m);
    double speed_of_sound_m = physics::ussa1976_v_sound(h_m);
    double true_airspeed_mps =
        std::sqrt(vx_body_mps * vx_body_mps + vy_body_mps * vy_body_mps
                  + vz_body_mps * vz_body_mps);
    double qbar_kgpms2 =
        0.5 * rho_interp_kgpm3 * true_airspeed_mps * true_airspeed_mps;
    double mach = true_airspeed_mps / speed_of_sound_m;

    // Angle of attack (alpha) and angle of sideslip (beta)
    double alpha_rad = std::atan2(vz_body_mps, vx_body_mps + 1E-9);
    double beta_rad  = std::asin(vy_body_mps / true_airspeed_mps);
    double s_alpha   = std::sin(alpha_rad);
    double c_alpha   = std::cos(alpha_rad);
    double s_beta    = std::sin(beta_rad);
    double c_beta    = std::cos(beta_rad);

    // double gz_ned_mps2 = 9.80665;
    double gz_ned_mps2 = physics::gravity_lookup(h_m);

    // Gravity in body coordinates
    double gx_body_mps2 = -s_theta * gz_ned_mps2;
    double gy_body_mps2 = s_phi * c_theta * gz_ned_mps2;
    double gz_body_mps2 = c_phi * c_theta * gz_ned_mps2;

    // Aerodynamic forces
    double drag_kgmps2 = 0; // CD_approx * qbar_kgpms2 * Aref_m2;
    double side_kgmps2 = 0;
    double lift_kgmps2 = 0;

    // External forces
    double Fx_body_kgmps2 =
        -(c_alpha * c_beta * drag_kgmps2 - c_alpha * s_beta * side_kgmps2
          - s_alpha * lift_kgmps2);
    double Fy_body_kgmps2 = -(s_beta * drag_kgmps2 + c_beta * side_kgmps2);
    double Fz_body_kgmps2 =
        -(s_alpha * c_beta * drag_kgmps2 - s_alpha * s_beta * side_kgmps2
          + c_alpha * lift_kgmps2);

    // External moments
    double l_body_kgm2ps2 = 0;
    double m_body_kgm2ps2 = 0;
    double n_body_kgm2ps2 = 0;

    double Den = Jxx_body_kgm2 * Jzz_body_kgm2 - Jxz_body_kgm2 * Jxz_body_kgm2;

    // x-axis velocity equation
    dx(0) = (1 / m_kg * Fx_body_kgmps2) + gx_body_mps2
            - (vz_body_mps * wy_body_rps) + (vy_body_mps * wz_body_rps);

    // y-axis velocity equation
    dx(1) = (1 / m_kg * Fy_body_kgmps2) + gy_body_mps2
            - (vx_body_mps * wz_body_rps) + (vz_body_mps * wx_body_rps);

    // z-axis velocity equation
    dx(2) = (1 / m_kg * Fz_body_kgmps2) + gz_body_mps2
            - (vy_body_mps * wx_body_rps) + (vx_body_mps * wy_body_rps);

    // roll equation
    dx(3) = (Jxz_body_kgm2 * (Jxx_body_kgm2 - Jyy_b_kgm2 + Jzz_body_kgm2)
                 * wx_body_rps * wy_body_rps
             - (Jzz_body_kgm2 * (Jzz_body_kgm2 - Jyy_b_kgm2)
                + Jxz_body_kgm2 * Jxz_body_kgm2)
                   * wy_body_rps * wz_body_rps
             + Jzz_body_kgm2 * l_body_kgm2ps2 + Jxz_body_kgm2 * n_body_kgm2ps2)
            / Den;

    // pitch equation
    dx(4) = ((Jzz_body_kgm2 - Jxx_body_kgm2) * wx_body_rps * wz_body_rps
             - Jxz_body_kgm2
                   * (wx_body_rps * wx_body_rps - wz_body_rps * wz_body_rps)
             + m_body_kgm2ps2)
            / Jyy_b_kgm2;

    // yaw equation
    dx(5) = ((Jxx_body_kgm2 * (Jxx_body_kgm2 - Jyy_b_kgm2)
              + Jxz_body_kgm2 * Jxz_body_kgm2)
                 * wx_body_rps * wy_body_rps
             - Jxz_body_kgm2 * (Jxx_body_kgm2 - Jyy_b_kgm2 + Jzz_body_kgm2)
                   * wy_body_rps * wz_body_rps
             + Jxz_body_kgm2 * l_body_kgm2ps2 + Jxx_body_kgm2 * n_body_kgm2ps2)
            / Den;

    // kinematic equations
    dx(6) = wx_body_rps + std::sin(phi_rad) * std::tan(theta_rad) * wy_body_rps
            + std::cos(phi_rad) * std::tan(theta_rad) * wz_body_rps;

    dx(7) = std::cos(phi_rad) * wy_body_rps - std::sin(phi_rad) * wz_body_rps;

    dx(8) = std::sin(phi_rad) / std::cos(theta_rad) * wy_body_rps
            + std::cos(phi_rad) / std::cos(theta_rad) * wz_body_rps;

    // position equations
    dx(9) = c_theta * c_psi * vx_body_mps
            + (-c_phi * s_psi + s_phi * s_theta * c_psi) * vy_body_mps
            + (s_phi * s_psi + c_phi * s_theta * c_psi) * vz_body_mps;

    dx(10) = c_theta * s_psi * vx_body_mps
             + (c_phi * c_psi + s_phi * s_theta * s_psi) * vy_body_mps
             + (-s_phi * c_psi + c_phi * s_theta * s_psi) * vz_body_mps;

    dx(11) = -s_theta * vx_body_mps + s_phi * c_theta * vy_body_mps
             + c_phi * c_theta * vz_body_mps;

    if (aux) {
        aux->altitude_m           = h_m;
        aux->speed_of_sound_mps   = speed_of_sound_m;
        aux->rho_kgpm3            = rho_interp_kgpm3;
        aux->mach                 = mach;
        aux->alpha_rad            = alpha_rad;
        aux->beta_rad             = beta_rad;
        aux->true_airspeed_mps    = true_airspeed_mps;
        aux->c_phi                = c_phi;
        aux->c_psi                = c_psi;
        aux->c_theta              = c_theta;
        aux->s_phi                = s_phi;
        aux->s_theta              = s_theta;
        aux->s_psi                = s_psi;
        aux->t_theta              = t_theta;
        aux->dcm_body2inertial_11 = c_theta * c_psi;
        aux->dcm_body2inertial_12 = -c_phi * s_psi + s_phi * s_theta * c_psi;
        aux->dcm_body2inertial_13 = s_phi * s_psi + c_phi * s_theta * c_psi;
        aux->dcm_body2inertial_21 = c_theta * s_psi;
        aux->dcm_body2inertial_22 = c_phi * c_psi + s_phi * s_theta * s_psi;
        aux->dcm_body2inertial_23 = -s_phi * c_psi + c_phi * s_theta * s_psi;
        aux->dcm_body2inertial_31 = -s_theta;
        aux->dcm_body2inertial_32 = s_phi * c_theta;
        aux->dcm_body2inertial_33 = c_phi * c_theta;

        double const c11 = aux->dcm_body2inertial_11;
        double const c12 = aux->dcm_body2inertial_12;
        double const c13 = aux->dcm_body2inertial_13;
        double const c21 = aux->dcm_body2inertial_21;
        double const c22 = aux->dcm_body2inertial_22;
        double const c23 = aux->dcm_body2inertial_23;
        double const c31 = aux->dcm_body2inertial_31;
        double const c32 = aux->dcm_body2inertial_32;
        double const c33 = aux->dcm_body2inertial_33;

        aux->vx_ned_mps = c11 * x(0) + c12 * x(1) + c13 * x(2);
        aux->vy_ned_mps = c21 * x(0) + c22 * x(1) + c23 * x(2);
        aux->vz_ned_mps = c31 * x(0) + c32 * x(1) + c33 * x(2);

        aux->roll_from_dcm_rad  = std::atan2(c32, c33);
        aux->pitch_from_dcm_rad = -std::asin(c31);
        aux->yaw_from_dcm_rad   = std::atan2(c21, c11);
    }

    return dx;
}
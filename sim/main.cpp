#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <math/Eigen/Core>
#include <vector>

Eigen::Matrix<double, 12, 1> flat_earth_eom(Eigen::Matrix<double, 12, 1> const& x) {
    Eigen::Matrix<double, 12, 1> dx;
    dx.setZero();

    double u_b_mps   = x(0);
    double v_b_mps   = x(1);
    double w_b_mps   = x(2);
    double p_b_rps   = x(3);
    double q_b_rps   = x(4);
    double r_b_rps   = x(5);
    double phi_rad   = x(6);
    double theta_rad = x(7);
    double psi_rad   = x(8);
    double p1_n_m    = x(9);
    double p2_n_m    = x(10);
    double p3_n_m    = x(11);

    double c_phi   = std::cos(phi_rad);
    double c_theta = std::cos(theta_rad);
    double c_psi   = std::cos(psi_rad);
    double s_phi   = std::sin(phi_rad);
    double s_theta = std::sin(theta_rad);
    double s_psi   = std::sin(psi_rad);
    double t_theta = std::tan(theta_rad);

    double in2m = 0.0254;

    double rho_lead_kgpm3 = 11300.0;
    double r_sphere_in    = 0.495;
    double r_sphere_m     = r_sphere_in * in2m;
    double CD_approx      = 0.5;

    double vol_sphere_m3 = 4.0 / 3.0 * M_PI * r_sphere_m * r_sphere_m * r_sphere_m;
    double m_sphere_kg   = rho_lead_kgpm3 * vol_sphere_m3;
    double J_sphere_kgm2 = 0.4 * m_sphere_kg * r_sphere_m * r_sphere_m;
    double Aref_m2       = M_PI * r_sphere_m * r_sphere_m;

    double m_kg       = rho_lead_kgpm3 * vol_sphere_m3;
    double Jxz_b_kgm2 = 0;
    double Jxx_b_kgm2 = J_sphere_kgm2;
    double Jyy_b_kgm2 = J_sphere_kgm2;
    double Jzz_b_kgm2 = J_sphere_kgm2;

    // current altitude
    double h_m = -p3_n_m;

    double rho_interp_kgpm3  = 1.20;
    double true_airspeed_mps = std::sqrt(u_b_mps * u_b_mps + v_b_mps * v_b_mps + w_b_mps * w_b_mps);
    double qbar_kgpms2       = 0.5 * rho_interp_kgpm3 * true_airspeed_mps * true_airspeed_mps;

    // Avoid divide by 0 errors
    double w_over_u;
    if (u_b_mps == 0 && w_b_mps == 0) {
        w_over_u = 0;
    } else {
        w_over_u = w_b_mps / u_b_mps;
    }

    double v_over_VT;
    if (true_airspeed_mps == 0 and v_b_mps == 0) {
        v_over_VT = 0;
    } else {
        v_over_VT = v_b_mps / true_airspeed_mps;
    }

    // Angle of attack (alpha) and angle of sideslip (beta)
    double alpha_rad = std::atan(w_over_u);
    double beta_rad  = std::asin(v_over_VT);
    double s_alpha   = std::sin(alpha_rad);
    double c_alpha   = std::cos(alpha_rad);
    double s_beta    = std::sin(beta_rad);
    double c_beta    = std::cos(beta_rad);

    double gz_n_mps2 = 9.81;

    // Gravity in body coordinates
    double gx_b_mps2 = -s_theta * gz_n_mps2;
    double gy_b_mps2 = s_phi * c_theta * gz_n_mps2;
    double gz_b_mps2 = c_phi * c_theta * gz_n_mps2;

    // Aerodynamic forces
    double drag_kgmps2 = CD_approx * qbar_kgpms2 * Aref_m2;
    double side_kgmps2 = 0;
    double lift_kgmps2 = 0;

    // External forces
    double Fx_b_kgmps2 = -(c_alpha * c_beta * drag_kgmps2 - c_alpha * s_beta * side_kgmps2 - s_alpha * lift_kgmps2);
    double Fy_b_kgmps2 = -(s_beta * drag_kgmps2 + c_beta * side_kgmps2);
    double Fz_b_kgmps2 = -(s_alpha * c_beta * drag_kgmps2 - s_alpha * s_beta * side_kgmps2 + c_alpha * lift_kgmps2);

    // External moments
    double l_b_kgm2ps2 = 0;
    double m_b_kgm2ps2 = 0;
    double n_b_kgm2ps2 = 0;

    double Den = Jxx_b_kgm2 * Jzz_b_kgm2 - Jxz_b_kgm2 * Jxz_b_kgm2;

    // x-axis velocity equation
    dx(0) = (1 / m_kg * Fx_b_kgmps2) + gx_b_mps2 - (w_b_mps * q_b_rps) + (v_b_mps * r_b_rps);

    // y-axis velocity equation
    dx(1) = (1 / m_kg * Fy_b_kgmps2) + gy_b_mps2 - (u_b_mps * r_b_rps) + (w_b_mps * p_b_rps);

    // z-axis velocity equation
    dx(2) = (1 / m_kg * Fz_b_kgmps2) + gz_b_mps2 - (v_b_mps * p_b_rps) + (u_b_mps * q_b_rps);

    // roll equation
    dx(3) = (Jxz_b_kgm2 * (Jxx_b_kgm2 - Jyy_b_kgm2 + Jzz_b_kgm2) * p_b_rps * q_b_rps -
             (Jzz_b_kgm2 * (Jzz_b_kgm2 - Jyy_b_kgm2) + Jxz_b_kgm2 * Jxz_b_kgm2) * q_b_rps * r_b_rps +
             Jzz_b_kgm2 * l_b_kgm2ps2 + Jxz_b_kgm2 * n_b_kgm2ps2) /
            Den;

    // pitch equation
    dx(4) = ((Jzz_b_kgm2 - Jxx_b_kgm2) * p_b_rps * r_b_rps - Jxz_b_kgm2 * (p_b_rps * p_b_rps - r_b_rps * r_b_rps) +
             m_b_kgm2ps2) /
            Jyy_b_kgm2;

    // yaw equation
    dx(5) = ((Jxx_b_kgm2 * (Jxx_b_kgm2 - Jyy_b_kgm2) + Jxz_b_kgm2 * Jxz_b_kgm2) * p_b_rps * q_b_rps +
             Jxz_b_kgm2 * (Jxx_b_kgm2 - Jyy_b_kgm2 + Jzz_b_kgm2) * q_b_rps * r_b_rps + Jxz_b_kgm2 * l_b_kgm2ps2 +
             Jxz_b_kgm2 * n_b_kgm2ps2) /
            Den;

    // kinematic equations
    dx(6) =
        p_b_rps + std::sin(phi_rad) * std::tan(theta_rad) * q_b_rps + std::cos(phi_rad) * std::tan(theta_rad) * r_b_rps;

    dx(7) = std::cos(phi_rad) * q_b_rps - std::sin(phi_rad) * r_b_rps;

    dx(8) = std::sin(phi_rad) / std::cos(theta_rad) * q_b_rps + std::cos(phi_rad) / std::cos(theta_rad) * r_b_rps;

    // position equations
    dx(9) = c_theta * c_psi * u_b_mps + (-c_phi * s_psi + s_phi * s_theta * c_psi) * v_b_mps +
            (s_phi * s_psi + c_phi * s_theta * c_psi) * w_b_mps;

    dx(10) = c_theta * s_psi * u_b_mps + (c_phi * c_psi + s_phi * s_theta * s_psi) * v_b_mps +
             (-s_phi * c_psi + c_phi * s_theta * s_psi) * w_b_mps;

    dx(11) = -s_theta * u_b_mps + s_phi * c_theta * v_b_mps + c_phi * c_theta * w_b_mps;

    return dx;
}

std::vector<double> arange(double start, double stop, double step) {
    std::vector<double> result;

    int n = static_cast<int>((stop - start) / step);
    for (int i = 0; i <= n; i++)
        result.push_back(start + i * step);

    return result;
}

void forward_euler(Eigen::Matrix<double, 12, Eigen::Dynamic>& x, std::vector<double> const& t_s, double h_s) {
    std::size_t const nt_s = t_s.size();

    for (std::size_t i = 1; i < nt_s; ++i) {
        x.col(i) = x.col(i - 1) + h_s * flat_earth_eom(x.col(i - 1));
    }
}

void export_to_csv(Eigen::Matrix<double, 12, Eigen::Dynamic> const& x, std::vector<double> const& t_s,
                   std::string const& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    size_t const nt_s = t_s.size();

    // Write header
    file << "time";
    for (int i = 0; i < x.rows(); ++i)
        file << ",x" << i;
    file << "\n";

    for (size_t k = 0; k < nt_s; ++k) {
        file << t_s[k];
        for (int i = 0; i < x.rows(); ++i)
            file << "," << x(i, k);
        file << "\n";
    }

    file.close();
}

int main() {
    std::printf("Running simulation...");
    // Initial conditions
    double u0_bf_mps  = 0.001;
    double v0_bf_mps  = 0;
    double w0_bf_mps  = 0;
    double p0_bf_rps  = 0;
    double q0_bf_rps  = 0;
    double r0_bf_rps  = 0;
    double phi0_rad   = 0 * M_PI / 180;
    double theta0_rad = 0 * M_PI / 180;
    double psi0_rad   = 0;
    double p10_n_m    = 0;
    double p20_n_m    = 0;
    double p30_n_m    = -906;

    Eigen::Matrix<double, 12, 1> x0 = (Eigen::Matrix<double, 12, 1>() << u0_bf_mps, v0_bf_mps, w0_bf_mps, p0_bf_rps,
                                       q0_bf_rps, r0_bf_rps, phi0_rad, theta0_rad, psi0_rad, p10_n_m, p20_n_m, p30_n_m)
                                          .finished();

    double t0_s = 0.0;
    double tf_s = 185;
    double h_s  = 0.01;

    // Numerical approximation
    std::vector<double> t_s = arange(t0_s, tf_s + h_s, h_s);
    std::size_t nt_s        = t_s.size();
    Eigen::Matrix<double, 12, Eigen::Dynamic> x;
    x.resize(12, nt_s);

    x.col(0) = x0;

    forward_euler(x, t_s, h_s);
    export_to_csv(x, t_s, "simulation_output.csv");

    std::printf("Simulation complete. Results saved\n");

    return 0;
}
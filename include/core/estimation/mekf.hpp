#pragma once

#include "core/math/matrix.hpp"
#include "core/math/quaternion.hpp"
#include "core/math/vector3f.hpp"

class MEKF {
public:
    MEKF(float P0, float sigma_gyro, float sigma_bias, float sigma_accel, float sigma_mag)
        : sigma_gyro(sigma_gyro), sigma_bias(sigma_bias), sigma_accel(sigma_accel), sigma_mag(sigma_mag) {
        q = Quaternion();
        b = Vector3f(0.0f, 0.0f, 0.0f);
        P = Matrix<6, 6>(P0);

        // Measurement noise
        R(0, 0) = R(1, 1) = R(2, 2) = this->sigma_accel * this->sigma_accel;
        R(3, 3) = R(4, 4) = R(5, 5) = this->sigma_mag * this->sigma_mag;

        G(0, 3) = G(1, 4) = G(2, 5) = -1.0f;

        I = Matrix<6, 6>(1.0f);
    }

    Quaternion const& get_quat() const { return q; }
    Vector3f const& get_bias() const { return b; }

    void predict(Vector3f const& gyro_meas, float dt);
    void update(Vector3f const& accel_in, Vector3f const& mag_in);

private:
    Vector3f G_REF;
    Vector3f M_REF;
    bool mag_initialized = false;

    Quaternion q = {1, 0, 0, 0};       // [w, x, y, z]
    Vector3f b   = {0.0f, 0.0f, 0.0f}; // gyro bias
    float sigma_gyro;
    float sigma_bias;
    float sigma_accel;
    float sigma_mag;

    Matrix<6, 6> P; // covariance
    Matrix<6, 6> Q; // process noise
    Matrix<6, 6> R; // accel + mag noise
    Matrix<6, 6> G; // process model

    Matrix<6, 6> I; // identity matrix for ease of operations

    // helpers
    void processCov(float dt);
    Matrix<6, 6> computeH() const;
    static Matrix<3, 3> skew3(Vector3f const& v);
    static Vector3f projectToHorizontal(Vector3f const& accel, Vector3f const& mag);
};

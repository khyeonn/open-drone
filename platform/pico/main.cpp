#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <stdio.h>

#include "estimation/mekf.hpp"
#include "sensors/mpu9250.hpp"

MEKF filter(0.1f, 0.1f, 0.1f, 0.1f, 0.1f);

int main() {
    stdio_init_all();
    sleep_ms(2000);

    printf("Initializing MPU9250.\n");
    MPU9250 imu;
    while (!imu.init()) {
        printf("MPU9250 could not be initialized.\n");
        sleep_ms(1000);
    };
    printf("MPU9250 initialized.\n");
    printf("Calibrating accelerometer. Keep sensor still.\n");
    imu.calibrate();
    printf("Accelerometer calibration done.\n");

    printf("Calibrating magnetometer. Rotate sensor smoothly.\n");
    sleep_ms(1500);
    imu.calibrate_mag();
    printf("Magnetometer calibration done.\n");
    printf("Pico Drone v1.1 Initialized!\n");
    printf("q_w,q_x,q_y,q_z,accel_x_g,accel_y_g,accel_z_g,gyro_x_dps,gyro_y_dps,gyro_z_dps,mag_x_ut,mag_y_ut,mag_z_ut,temp_c\n");

    absolute_time_t last_time = get_absolute_time();
    while (true) {

        absolute_time_t current_time = get_absolute_time();
        float dt                     = absolute_time_diff_us(last_time, current_time) / 1e6f;

        if (dt < 1e-6f)
            continue;
        last_time = current_time;

        imu.read_accel_gyro_raw();
        imu.read_mag_raw();

        imu.convert_accel_raw();
        imu.convert_gyro_raw();
        imu.convert_temp_raw();
        imu.convert_mag_raw();

        Vector3f accel = imu.get_accel();
        Vector3f gyro  = imu.get_gyro();
        Vector3f mag   = imu.get_mag();
        float temp     = imu.get_temp();

        filter.predict(gyro, dt);
        filter.update(accel, mag);

        printf("%.4f,%.4f,%.4f,%.4f,%.3f,%.3f,%.3f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
               filter.get_quat().w, filter.get_quat().x, filter.get_quat().y, filter.get_quat().z, //
               accel.x, accel.y, accel.z,                                                          //
               gyro.x, gyro.y, gyro.z,                                                             //
               mag.x, mag.y, mag.z,                                                                //
               temp);

        sleep_ms(50);
    }
    return 0;
}
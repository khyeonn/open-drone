#pragma once

#include <array>

namespace physics {

struct AtmosphereRowData {
    double altitude;
    double temperature;
    double pressure;
    double density;
    double speed_of_sound;
};

constexpr int ATM_TABLE_SIZE = 861; // 0 - 86 km @ 100m resolution

extern std::array<AtmosphereRowData, ATM_TABLE_SIZE> const ATM_TABLE;
extern std::array<double, ATM_TABLE_SIZE> const GRAVITY_TABLE;

AtmosphereRowData ussa1976_lookup(double altitude);
double ussa1976_density(double altitude);
double ussa1976_v_sound(double altitude);
double gravity_lookup(double altitude);

} // namespace physics

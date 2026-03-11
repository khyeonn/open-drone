#include "physics/ussa1976.hpp"
#include <algorithm>
#include <cmath>

namespace physics {

constexpr double g0    = 9.80665;
constexpr double R     = 287.05287;
constexpr double Gamma = 1.4;
constexpr double Re    = 6378137.0;
constexpr double DZ    = 100.0;

struct Layer {
    double h;
    double T;
    double P;
    double L;
};

constexpr Layer layers[] = {
    {  0.0, 288.15, 101325.0, -0.0065},
    {11000, 216.65, 22632.06,     0.0},
    {20000, 216.65, 5474.889,   0.001},
    {32000, 228.65, 868.0187,  0.0028},
    {47000, 270.65, 110.9063,     0.0},
    {51000, 270.65, 66.93887, -0.0028},
    {71000, 214.65,  3.95642,  -0.002}
};

constexpr physics::AtmosphereRowData compute_point(double h) {
    Layer const* L = &layers[0];

    constexpr std::size_t NUM_LAYERS = sizeof(layers) / sizeof(layers[0]);
    for (std::size_t i = 0; i < NUM_LAYERS; ++i) {
        if (h >= layers[i].h)
            L = &layers[i];
    }

    double const T = (L->L == 0.0) ? L->T : L->T + L->L * (h - L->h);
    double const P = (L->L == 0.0)
                         ? L->P * std::exp(-g0 * (h - L->h) / (R * T))
                         : L->P * std::pow(T / L->T, -g0 / (L->L * R));

    double rho = P / (R * T);
    double a   = std::sqrt(Gamma * R * T);

    return {h, T, P, rho, a};
}

constexpr double compute_gravity(double h) {
    double const r = Re + h;
    return g0 * (Re * Re) / (r * r);
}

constexpr std::array<physics::AtmosphereRowData, physics::ATM_TABLE_SIZE>
generate_table() {
    std::array<physics::AtmosphereRowData, physics::ATM_TABLE_SIZE> table{};

    for (std::size_t i = 0; i < physics::ATM_TABLE_SIZE; ++i) {
        double altitude = i * 100.0;

        table[i] = compute_point(altitude);
    }

    return table;
}

constexpr std::array<double, ATM_TABLE_SIZE> generate_gravity_table() {
    std::array<double, ATM_TABLE_SIZE> table{};

    for (std::size_t i = 0; i < ATM_TABLE_SIZE; ++i) {
        double const altitude = static_cast<double>(i) * DZ;

        table[i] = compute_gravity(altitude);
    }

    return table;
}

std::array<physics::AtmosphereRowData, physics::ATM_TABLE_SIZE> const
    ATM_TABLE = generate_table();
std::array<double, ATM_TABLE_SIZE> const GRAVITY_TABLE =
    generate_gravity_table();

physics::AtmosphereRowData ussa1976_lookup(double altitude) {
    altitude = std::clamp(altitude, 0.0, 86000.0);

    constexpr double inv_step = 1.0 / 100.0;

    double const x = altitude * inv_step;
    int const i    = static_cast<int>(x);

    if (i >= physics::ATM_TABLE_SIZE - 1) {
        return ATM_TABLE[physics::ATM_TABLE_SIZE - 1];
    }

    double const t = x - static_cast<double>(i);

    physics::AtmosphereRowData const& a = ATM_TABLE[i];
    physics::AtmosphereRowData const& b = ATM_TABLE[i + 1];

    return {altitude, a.temperature + t * (b.temperature - a.temperature),
            a.pressure + t * (b.pressure - a.pressure),
            a.density + t * (b.density - a.density),
            a.speed_of_sound + t * (b.speed_of_sound - a.speed_of_sound)};
}

double ussa1976_density(double altitude) {
    altitude = std::clamp(altitude, 0.0, 86000.0);

    constexpr double inv_step = 1.0 / 100.0;

    double const x = altitude * inv_step;
    int const i    = static_cast<int>(x);

    if (i >= physics::ATM_TABLE_SIZE - 1) {
        return ATM_TABLE[physics::ATM_TABLE_SIZE - 1].density;
    }

    double const t = x - static_cast<double>(i);

    physics::AtmosphereRowData const& a = ATM_TABLE[i];
    physics::AtmosphereRowData const& b = ATM_TABLE[i + 1];

    return a.density + t * (b.density - a.density);
}

double ussa1976_v_sound(double altitude) {
    altitude = std::clamp(altitude, 0.0, 86000.0);

    constexpr double inv_step = 1.0 / 100.0;

    double const x = altitude * inv_step;
    int const i    = static_cast<int>(x);

    if (i >= physics::ATM_TABLE_SIZE - 1) {
        return ATM_TABLE[physics::ATM_TABLE_SIZE - 1].speed_of_sound;
    }

    double const t = x - static_cast<double>(i);

    physics::AtmosphereRowData const& a = ATM_TABLE[i];
    physics::AtmosphereRowData const& b = ATM_TABLE[i + 1];

    return a.speed_of_sound + t * (b.speed_of_sound - a.speed_of_sound);
}

double gravity_lookup(double altitude) {
    altitude = std::clamp(altitude, 0.0, 86000.0);

    constexpr double inv_step = 1.0 / DZ;

    double const x      = altitude * inv_step;
    std::size_t const i = static_cast<std::size_t>(x);

    if (i >= ATM_TABLE_SIZE - 1) {
        return GRAVITY_TABLE[ATM_TABLE_SIZE - 1];
    }

    double const t = x - static_cast<double>(i);

    double const g_a = GRAVITY_TABLE[i];
    double const g_b = GRAVITY_TABLE[i + 1];

    return g_a + t * (g_b - g_a);
}

} // namespace physics
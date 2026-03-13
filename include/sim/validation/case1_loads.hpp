#pragma once

#include "sim/types/aux_data.hpp"
#include "sim/types/drone_state.hpp"
#include "sim/types/external_loads.hpp"
#include "sim/types/flight_conditions.hpp"
#include <cmath>

namespace sim::loads {

template <typename Vehicle> class Case1Loads {
public:
    types::ExternalLoads compute(Vehicle const& vehicle, types::State const& x,
                                 types::FlightCondition const& fc,
                                 types::AuxData* aux = nullptr) const {
        types::ExternalLoads loads{};

        loads.fx_b_N = 0.0;
        loads.fy_b_N = 0.0;
        loads.fz_b_N = 0.0;

        return loads;
    }
};

} // namespace sim::loads
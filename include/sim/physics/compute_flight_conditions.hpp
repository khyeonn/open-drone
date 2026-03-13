#pragma once

#include "sim/types/drone_state.hpp"
#include "sim/types/flight_conditions.hpp"

namespace sim::physics {

types::FlightCondition compute_flight_condition(types::State const& x);

}
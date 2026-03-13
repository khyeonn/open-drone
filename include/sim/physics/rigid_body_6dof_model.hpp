#pragma once

#include "sim/physics/compute_flight_conditions.hpp"
#include "sim/physics/flat_earth_eom.hpp"
#include "sim/types/aux_data.hpp"
#include "sim/types/drone_state.hpp"
#include "sim/types/external_loads.hpp"
#include "sim/types/flight_conditions.hpp"

namespace sim::physics {

template <typename Vehicle, typename LoadModel> class Model {
public:
    Vehicle vehicle;
    LoadModel load_model;

    types::State derivative(types::State const& x,
                            types::AuxData* aux = nullptr) const {

        types::FlightCondition const fc = compute_flight_condition(x);
        types::ExternalLoads const loads =
            load_model.compute(vehicle, x, fc, aux);

        return physics::derivative(vehicle, x, loads, aux);
    }
};

} // namespace sim::physics
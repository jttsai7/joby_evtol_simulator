#include "Aircraft.h"
#include <algorithm> // std::min
#include <cmath>     // std::abs
#include <cstdlib>   // rand

Aircraft::Aircraft(CompanyType type)
    : type_(type),
      config_(AircraftConfig::GetConfig(type)),
      current_battery_kwh_(config_.battery_capacity_kwh) // Start full
{
}

void Aircraft::update(double dt_hours) {
    double remaining_time = dt_hours;

    // Precision Loop:
    // If a state change happens mid-step.
    // Example: If the step is 1.0 minute, and battery dies at t=0.7 min,
    // we must process the remaining 0.3 min in the NEW state (Waiting).
    // Epsilon (1e-7) prevents infinite loops due to float errors.
    while (remaining_time > 1e-7) {
        
        double time_consumed = 0.0;

        switch (state_) {
            case AircraftState::Flying:
                time_consumed = process_flying(remaining_time);
                break;
                
            case AircraftState::Waiting:
                // Idle state: just consumes time
                stats_wait_time_hours_ += remaining_time;
                time_consumed = remaining_time; 
                break;
                
            case AircraftState::Charging:
                time_consumed = process_charging(remaining_time);
                break;
        }

        remaining_time -= time_consumed;
    }
}

double Aircraft::process_flying(double available_time) {
    // 1. Calc Power (kW) = Usage (kWh/mi) * Speed (mph)
    double power_kw = config_.energy_use_kwh_mile * config_.cruise_speed_mph;
    
    // 2. Calc Endurance
    double max_flight_time = current_battery_kwh_ / power_kw;

    // 3. Determine actual time we can fly in this step
    double actual_flight_time = std::min(available_time, max_flight_time);

    // 4. Update Stats & Physics
    stats_flight_time_hours_ += actual_flight_time;
    double distance = actual_flight_time * config_.cruise_speed_mph;
    stats_passenger_miles_ += distance * config_.passenger_count;
    
    current_battery_kwh_ -= (power_kw * actual_flight_time);

    // 5. Check Faults (based on actual flight duration)
    check_faults(actual_flight_time);

    // 6. Handle Depletion
    // Spec: "Instantaneously is in line right when it runs out"
    if (current_battery_kwh_ <= 1e-4) {
        current_battery_kwh_ = 0.0;
        state_ = AircraftState::Waiting; // Transition
    }

    return actual_flight_time;
}

double Aircraft::process_charging(double available_time) {
    // Linear charging model
    double charge_rate_kw = config_.battery_capacity_kwh / config_.time_to_charge_hours;
    
    // Calc time needed to reach 100%
    double needed_energy = config_.battery_capacity_kwh - current_battery_kwh_;
    double time_to_full = needed_energy / charge_rate_kw;

    double actual_charge_time = std::min(available_time, time_to_full);

    stats_charge_time_hours_ += actual_charge_time;
    current_battery_kwh_ += charge_rate_kw * actual_charge_time;

    // Handle Full Charge
    // Spec: "Instantaneously reaches Cruise Speed"
    if (current_battery_kwh_ >= config_.battery_capacity_kwh - 1e-4) {
        current_battery_kwh_ = config_.battery_capacity_kwh;
        state_ = AircraftState::Flying; // Transition
    }

    return actual_charge_time;
}

void Aircraft::check_faults(double dt_hours) {
    // Simple Monte Carlo simulation. 
    // For Phase 3/Testability, consider injecting a random provider.
    double random_val = static_cast<double>(std::rand()) / RAND_MAX;
    if (random_val < (config_.fault_prob_per_hour * dt_hours)) {
        stats_fault_count_++;
    }
}

void Aircraft::start_charging() {
    if (state_ == AircraftState::Waiting) {
        state_ = AircraftState::Charging;
    }
}

bool Aircraft::is_fully_charged() const {
    return current_battery_kwh_ >= config_.battery_capacity_kwh;
}

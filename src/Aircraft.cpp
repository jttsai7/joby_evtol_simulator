#include "Aircraft.h"
#include <algorithm>
#include <random>

Aircraft::Aircraft(CompanyType type, std::shared_ptr<ChargerPool> charger_pool)
    : type_(type),
      config_(AircraftConfig::GetConfig(type)),
      current_battery_kwh_(config_.battery_capacity_kwh),
      charger_pool_(charger_pool),
      dist_0_1_(0.0, 1.0) 
{
    // Thread-safe RNG initialization. 
    // Using a local random_device to seed the Mersenne Twister engine per aircraft instance.
    std::random_device rd;
    rng_.seed(rd());
}


// Core simulation loop for a single aircraft.
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
                time_consumed = process_waiting(remaining_time);
                break;
            case AircraftState::Charging:
                time_consumed = process_charging(remaining_time);
                break;
        }
        remaining_time -= time_consumed;
    }
    
    /**
     * Increment tick count AFTER the precision loop completes.
     * This represents one successful 'wake-up' cycle where the full
     * duration of dt_hours has been accounted for across one or more states.
     */
    stats_.completed_ticks++;
}

// Energy consumption and passenger-mile accumulation during flight.
double Aircraft::process_flying(double available_time) {
    // 1. Calc Power (kW) = Usage (kWh/mi) * Speed (mph)
    double power_kw = config_.energy_use_kwh_mile * config_.cruise_speed_mph;
    
    // 2. Calc Endurance
    double max_flight_time = current_battery_kwh_ / power_kw;

    // 3. Determine actual time we can fly in this step
    double actual = std::min(available_time, max_flight_time);

    // 4. Update Stats & Physics
    stats_.flight_time_hours += actual;
    stats_.passenger_miles += actual * config_.cruise_speed_mph * config_.passenger_count;
    
    current_battery_kwh_ -= (power_kw * actual);
    
    // 5. Check Faults (based on actual flight duration)
    check_faults(actual);

    // 6. By transitioning to 'Waiting', the aircraft enters the resource contention loop
    // governed by the ChargerPool semaphore.
    if (current_battery_kwh_ <= 1e-4) {
        current_battery_kwh_ = 0.0;
        state_ = AircraftState::Waiting;
    }
    
    return actual;
}

// Logic for resource acquisition. Attempts to secure a charger from the semaphore pool.
double Aircraft::process_waiting(double available_time) {
    // Non-blocking attempt to acquire a charger from the shared pool
    if (charger_pool_->try_acquire()) {
        state_ = AircraftState::Charging;
        
        // Return 0.0 time consumed to allow the Charging logic to utilize the 
        // remaining time in the current tick immediately (seamless transition).
        return 0.0; 
    }
    
    // If no chargers are available, the entire time step is spent waiting
    stats_.wait_time_hours += available_time;
    return available_time;
}

// Logic for battery restoration. Returns the charger to the pool once full.
double Aircraft::process_charging(double available_time) {
    // Linear charging model
    double charge_rate_kw = config_.battery_capacity_kwh / config_.time_to_charge_hours;
    // Calc time needed to reach 100%
    double energy_needed = config_.battery_capacity_kwh - current_battery_kwh_;
    double time_to_full = energy_needed / charge_rate_kw;

    double actual = std::min(available_time, time_to_full);

    stats_.charge_time_hours += actual;
    current_battery_kwh_ += charge_rate_kw * actual;

    // State Transition: If battery reaches full capacity, resume flying
    if (current_battery_kwh_ >= config_.battery_capacity_kwh - 1e-4) {
        current_battery_kwh_ = config_.battery_capacity_kwh;
        state_ = AircraftState::Flying;
        
        // Release the charger resource back to the pool for other aircraft
        charger_pool_->release();
    }
    return actual;
}

// Monte Carlo simulation of component faults per hour of flight.
void Aircraft::check_faults(double dt_hours) {
    // Probability check: rand[0,1] < (fault_rate_per_hour * hours_flown)
    if (dist_0_1_(rng_) < (config_.fault_prob_per_hour * dt_hours)) {
        stats_.fault_count++;
    }
}

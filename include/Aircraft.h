#pragma once

#include "AircraftConfig.h"
#include "ChargerPool.h"
#include "AircraftStats.h"
#include <memory>
#include <random>

enum class AircraftState {
    Flying,   // Airborne and consuming battery
    Waiting,  // In queue for an available charger
    Charging  // Occupying a charger and restoring battery
};

// Manages state, physics, and statistics for a single eVTOL.
// Implements a precision state machine that handles mid-step transitions.
class Aircraft {
public:
    Aircraft(CompanyType type, std::shared_ptr<ChargerPool> charger_pool);

    // Core simulation step. 
    // Handles state transitions even if they occur in the middle of dt_hours.
    void update(double dt_hours);

    // --- State & Metadata ---
    const AircraftStats& get_stats() const { return stats_; }
    AircraftState get_state() const { return state_; }
    const std::string& get_name() const { return config_.name; }
    CompanyType get_type() const { return type_; }
    // Debug helper
    double get_battery_level() const { return current_battery_kwh_; }

private:
    // Internal processors: they return the 'actual time consumed' in that state.
    // This allows the main update loop to handle the remaining time in the next state.
    double process_flying(double available_time);
    double process_waiting(double available_time);
    double process_charging(double available_time);
    
    // Evaluates fault probability based on flight duration
    void check_faults(double dt_hours);

    CompanyType type_;
    const AircraftConfig& config_;
    std::shared_ptr<ChargerPool> charger_pool_;

    AircraftState state_ = AircraftState::Flying;
    double current_battery_kwh_;

    // Performance Metrics
    AircraftStats stats_;

    // Thread-local random number generation components
    std::mt19937 rng_;
    std::uniform_real_distribution<double> dist_0_1_;
};

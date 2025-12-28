#pragma once
#include "AircraftConfig.h"

enum class AircraftState {
    Flying,
    Waiting, // Queued for charger
    Charging
};

 // Manages state, physics, and statistics for a single eVTOL.
 // Implements a precision state machine that handles mid-step transitions.
class Aircraft {
public:
    explicit Aircraft(CompanyType type);

    // Core simulation step. 
    // Handles state transitions even if they occur in the middle of dt_hours.
    void update(double dt_hours);

    // Triggered by the Scheduler (Phase 3) when resources are available.
    void start_charging();

    // Resource check.
    bool is_fully_charged() const;

    // --- State & Metadata ---
    AircraftState get_state() const { return state_; }
    CompanyType get_type() const { return type_; }
    const std::string& get_name() const { return config_.name; }

    // --- KPIs ---
    double get_flight_time_hours() const { return stats_flight_time_hours_; }
    double get_charge_time_hours() const { return stats_charge_time_hours_; }
    double get_wait_time_hours() const { return stats_wait_time_hours_; }
    double get_passenger_miles() const { return stats_passenger_miles_; }
    int get_fault_count() const { return stats_fault_count_; }

    // Debug helper
    double get_battery_level() const { return current_battery_kwh_; }

private:
    // Internal processors: they return the 'actual time consumed' in that state.
    // This allows the main update loop to handle the remaining time in the next state.
    double process_flying(double available_time);
    double process_charging(double available_time);
    
    void check_faults(double dt_hours);

    CompanyType type_;
    const AircraftConfig& config_;

    AircraftState state_ = AircraftState::Flying; // Spec: Starts airborne
    double current_battery_kwh_;

    // Accumulated Statistics
    double stats_flight_time_hours_ = 0.0;
    double stats_charge_time_hours_ = 0.0;
    double stats_wait_time_hours_ = 0.0;
    double stats_passenger_miles_ = 0.0;
    int stats_fault_count_ = 0;
};

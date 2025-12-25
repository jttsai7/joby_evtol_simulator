#pragma once

#include "VehicleConfig.h"

namespace Joby {

    // Possible operational states of an eVTOL
    enum class VehicleState { Idle, Flying, Charging, Fault };

    class Vehicle {
    public:
        Vehicle(CompanyType type, int id);

        // Calculates energy consumption and charging progress
        void processFlight(double minutes);
        void processCharging(double minutes);

        // Check if the battery is topped off
        bool isFull() const;

        // Metrics and State accessors
        int getId() const { return id_; }
        std::string getName() const { return spec_.name; }
        VehicleState getState() const { return state_; }
        double getBatteryLevel() const { return currentBatteryKWh_; }
        double getTotalDistance() const { return totalDistanceMiles_; }
        int getCompletedTrips() const { return completedTrips_; }

    private:
        const int id_;
        const VehicleSpec& spec_; // Reference to the static config for this model
        
        VehicleState state_;
        double currentBatteryKWh_;
        
        // Cumulative stats for final reporting
        double totalFlightTimeHours_;
        double totalDistanceMiles_;
        int completedTrips_;
    };

} // namespace Joby

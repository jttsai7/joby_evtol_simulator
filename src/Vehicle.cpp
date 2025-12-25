#include "Vehicle.h"

namespace Joby {

    Vehicle::Vehicle(CompanyType type, int id) 
        : id_(id), 
          spec_(VehicleConfig::getSpec(type)),
          state_(VehicleState::Idle),
          currentBatteryKWh_(spec_.batteryCapacityKWh),
          totalFlightTimeHours_(0.0),
          totalDistanceMiles_(0.0),
          completedTrips_(0) {
    }

    // Logic for energy drain based on distance traveled during a time slice
    void Vehicle::processFlight(double minutes) {
        state_ = VehicleState::Flying;
        double hours = minutes / 60.0;
        double distance = hours * spec_.cruiseSpeedMph;
        double energyUsed = distance * spec_.energyUseAtCruiseKWhPerMile;

        currentBatteryKWh_ -= energyUsed;
        if (currentBatteryKWh_ < 0) currentBatteryKWh_ = 0; // Clamp to zero

        totalFlightTimeHours_ += hours;
        totalDistanceMiles_ += distance;
    }

    // Logic for battery replenishment based on the model's specific charging power
    void Vehicle::processCharging(double minutes) {
        state_ = VehicleState::Charging;
        double hours = minutes / 60.0;
        
        // Power (kW) = Total Capacity / Hours needed to reach full charge
        double chargeRateKW = spec_.batteryCapacityKWh / spec_.timeToChargeHours;
        currentBatteryKWh_ += (chargeRateKW * hours);

        // Transition back to Idle once battery is full
        if (currentBatteryKWh_ >= spec_.batteryCapacityKWh) {
            currentBatteryKWh_ = spec_.batteryCapacityKWh;
            state_ = VehicleState::Idle;
            completedTrips_++; // Consider one full cycle a completed mission
        }
    }

    bool Vehicle::isFull() const { return currentBatteryKWh_ >= spec_.batteryCapacityKWh; }

} // namespace Joby

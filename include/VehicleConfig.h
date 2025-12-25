#pragma once

#include <string>
#include <vector>
#include <map>

namespace Joby {

    // 5 specific aircraft manufacturers
    enum class CompanyType { Alpha, Beta, Charlie, Delta, Echo };

    // Hardware specs
    struct VehicleSpec {
        std::string name;
        double cruiseSpeedMph;
        double batteryCapacityKWh;
        double timeToChargeHours;
        double energyUseAtCruiseKWhPerMile;
        int passengerCount;
        double faultProbabilityPerHour;
    };

    class VehicleConfig {
    public:
        // Returns the hardware specification for a given company.
        static const VehicleSpec& getSpec(CompanyType type) {
            static const std::map<CompanyType, VehicleSpec> specs = {
                {CompanyType::Alpha,   {"Alpha",   120, 320, 0.60, 1.6, 4, 0.25}},
                {CompanyType::Beta,    {"Beta",    100, 100, 0.20, 1.5, 5, 0.10}},
                {CompanyType::Charlie, {"Charlie", 160, 220, 0.80, 2.2, 3, 0.05}},
                {CompanyType::Delta,   {"Delta",    90, 120, 0.62, 0.8, 2, 0.22}},
                {CompanyType::Echo,    {"Echo",     30, 150, 0.30, 5.8, 2, 0.61}}
            };
            return specs.at(type);
        }

        // Helper to iterate through all company types during simulation setup
        static const std::vector<CompanyType>& getAllTypes() {
            static const std::vector<CompanyType> types = {
                CompanyType::Alpha, CompanyType::Beta, CompanyType::Charlie, 
                CompanyType::Delta, CompanyType::Echo
            };
            return types;
        }
    };

} // namespace Joby

#pragma once
#include <string>
#include <array>
#include <stdexcept>

// Company types
enum class CompanyType {
    Alpha = 0,
    Beta,
    Charlie,
    Delta,
    Echo,
    Count // Helper for array sizing
};

struct AircraftConfig {
    std::string name;
    double cruise_speed_mph;
    double battery_capacity_kwh;
    double time_to_charge_hours;
    double energy_use_kwh_mile;
    int passenger_count;
    double fault_prob_per_hour;

    // Static Lookup for aircraft configuration
    static const AircraftConfig& GetConfig(CompanyType type) {
        static const std::array<AircraftConfig, static_cast<int>(CompanyType::Count)> configs = {{
            // Name,    Speed, Cap,  ChgTime, Usage, Pax, FaultRate
            {"Alpha",   120,   320,  0.60,    1.6,   4,   0.25},
            {"Beta",    100,   100,  0.20,    1.5,   5,   0.10},
            {"Charlie", 160,   220,  0.80,    2.2,   3,   0.05},
            {"Delta",    90,   120,  0.62,    0.8,   2,   0.22},
            {"Echo",     30,   150,  0.30,    5.8,   2,   0.61}
        }};

        int index = static_cast<int>(type);
        if (index < 0 || index >= configs.size()) {
            throw std::runtime_error("Invalid CompanyType index");
        }
        return configs[index];
    }
};

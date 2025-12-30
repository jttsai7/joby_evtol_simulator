#include "Simulator.h"
#include <iostream>

/**
 * Entry point for the eVTOL Simulation Project.
 * Configured per problem requirements: 20 vehicles, 3 chargers, 3-minute runtime.
 */
int main() {
    try {
        // Configuration: 20 random aircraft, 3 chargers, 3.0 minutes real-world time
        const int TOTAL_VEHICLES = 20;
        const int TOTAL_CHARGERS = 3;
        const double SIM_DURATION_MIN = 3.0;

        std::cout << "Joby Aviation eVTOL Simulation Engine" << std::endl;
        std::cout << "--------------------------------------" << std::endl;

        // Initialize and execute simulation
        Simulator app(TOTAL_VEHICLES, TOTAL_CHARGERS, SIM_DURATION_MIN);
        app.run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error during simulation: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

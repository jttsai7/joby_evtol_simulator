#include "Simulator.h"
#include <iostream>
#include <string>

/**
 * Entry point for the eVTOL Simulation Project.
 * Configured per problem requirements: 20 vehicles, 3 chargers, 3-minute runtime.
 */
int main(int argc, char* argv[]) {
    try {
        const int TOTAL_VEHICLES = 20;
        const int TOTAL_CHARGERS = 3;
        const double SIM_DURATION_MIN = 3.0;

        // Default to FIXED mode per original architecture
        Simulator::TimingMode mode = Simulator::TimingMode::FIXED;

        // Enhancement: Support '--compensated' flag for precision timing
        if (argc > 1 && std::string(argv[1]) == "--compensated") {
            mode = Simulator::TimingMode::COMPENSATED;
        }

        std::cout << "Joby Aviation eVTOL Simulation Engine" << std::endl;
        std::cout << "Timing Mode: " << (mode == Simulator::TimingMode::FIXED ? "FIXED" : "COMPENSATED") << std::endl;
        std::cout << "--------------------------------------" << std::endl;

        // Initialize with all required parameters including TimingMode
        Simulator app(TOTAL_VEHICLES, TOTAL_CHARGERS, SIM_DURATION_MIN, mode);
        app.run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error during simulation: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

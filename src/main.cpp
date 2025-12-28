#include <iostream>
#include <thread>
#include <vector>
#include "Aircraft.h" 

int main() {
    // ==========================================
    // PHASE 1: Environment & Concurrency Check
    // ==========================================
    // Just verifying that C++20 and threading headers are working
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << "eVTOL Simulation Engine Starting..." << std::endl;
    std::cout << "Detected " << n << " concurrent threads supported." << std::endl;

    // ==========================================
    // PHASE 2: Integration & Logic Check
    // ==========================================
    try {
        Aircraft alpha(CompanyType::Alpha);
        
        // Log initial state to console
        std::cout << "[OK] Created Aircraft: " << alpha.get_name() << std::endl;
        std::cout << "     - Battery: " << alpha.get_battery_level() << " kWh" << std::endl;
        std::cout << "     - State: " << static_cast<int>(alpha.get_state()) << " (0=Flying)" << std::endl;

        // Dry run: Advance simulation by 0.5 hours to verify logic flow
        alpha.update(0.5); 
        std::cout << "[OK] Ran update(0.5 hours)" << std::endl;
        
        // Check if battery drained as expected
        std::cout << "     - New Battery: " << alpha.get_battery_level() << " kWh" << std::endl;

    } catch (const std::exception& e) {
        // Catch runtime errors during integration (e.g. invalid config)
        std::cerr << "[Error] Integration failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

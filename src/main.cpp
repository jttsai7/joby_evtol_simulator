#include <iostream>
#include "Vehicle.h"

int main() {
    // Sanity check to ensure our physics logic is linked correctly.
    // We'll use "Charlie" as a representative model for this quick test.
    Joby::Vehicle testVehicle(Joby::CompanyType::Charlie, 1);

    std::cout << "--- Joby eVTOL Simulator: Phase 2 Sanity Check ---" << std::endl;
    std::cout << "Vehicle: " << testVehicle.getName() << " (ID: " << testVehicle.getId() << ")" << std::endl;
    std::cout << "Initial Battery: " << testVehicle.getBatteryLevel() << " kWh" << std::endl;

    // Simulate a 30-minute flight segment
    std::cout << "\nExecuting 30-minute flight..." << std::endl;
    testVehicle.processFlight(30.0);

    std::cout << "Post-flight Battery: " << testVehicle.getBatteryLevel() << " kWh" << std::endl;
    std::cout << "Distance Covered: " << testVehicle.getTotalDistance() << " miles" << std::endl;

    std::cout << "\nPhase 2 Logic Linkage: SUCCESS" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    return 0;
}

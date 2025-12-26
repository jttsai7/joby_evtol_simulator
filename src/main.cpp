#include <iostream>
#include <thread>
#include <vector>

// Sanity check for the environment
int main() {
    // Just verifying that C++20 and threading headers are available
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << "eVTOL Simulation Engine Starting..." << std::endl;
    std::cout << "Detected " << n << " concurrent threads supported." << std::endl;

    // TODO: Phase 2 - Initialize Config
    // TODO: Phase 3 - Start Simulation Loop
    
    return 0;
}

#pragma once

#include <vector>
#include <memory>
#include <atomic>
#include "Aircraft.h"
#include "ChargerPool.h"

/**
 * Orchestrates the eVTOL simulation lifecycle, thread management, 
 * and shared resource arbitration.
 */
class Simulator {
public:
    Simulator(int num_aircraft, int num_chargers, double duration_minutes);

    // Starts the simulation and blocks until the duration is reached
    void run();

private:
    // Core logic executed by each aircraft thread
    static void worker_thread(std::shared_ptr<Aircraft> aircraft, 
                              std::atomic<bool>& running, 
                              double sim_dt_hours, 
                              int tick_ms);

    int num_aircraft_;
    double duration_minutes_;
    
    // Shared resources and vehicle fleet
    std::shared_ptr<ChargerPool> charger_pool_;
    std::vector<std::shared_ptr<Aircraft>> fleet_;
};

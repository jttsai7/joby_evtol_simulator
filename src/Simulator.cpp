#include "Simulator.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <mutex>

// Mapping: 1s real-world = 1m simulation. 10ms tick ensures high resolution.
static const int TICK_MS = 10;
static const double SIM_SPEEDUP = 60.0; 
static const double SIM_DT_HOURS = (double(TICK_MS) / 1000.0) * SIM_SPEEDUP / 3600.0;

// Mutex to prevent console log interleaving from multiple aircraft threads.
static std::mutex print_mutex;

Simulator::Simulator(int num_aircraft, int num_chargers, double duration_minutes)
    : num_aircraft_(num_aircraft), duration_minutes_(duration_minutes) 
{
    charger_pool_ = std::make_shared<ChargerPool>(num_chargers);
    
    // Fixed seed for deterministic vehicle distribution across different runs.
    std::mt19937 factory_rng(12345);
    std::uniform_int_distribution<int> type_dist(0, 4);

    fleet_.reserve(num_aircraft_);
    for (int i = 0; i < num_aircraft_; ++i) {
        CompanyType type = static_cast<CompanyType>(type_dist(factory_rng));
        fleet_.push_back(std::make_shared<Aircraft>(type, charger_pool_));
    }
}

void Simulator::run() {
    std::cout << "Deploying " << num_aircraft_ << " eVTOL aircraft threads..." << std::endl;
    
    std::vector<std::thread> threads;
    std::atomic<bool> running{true};

    // Spin up one thread per aircraft to simulate concurrent behavior.
    for (auto& aircraft : fleet_) {
        threads.emplace_back(worker_thread, aircraft, std::ref(running), SIM_DT_HOURS, TICK_MS);
    }

    auto start_time = std::chrono::steady_clock::now();
    while (true) {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - start_time;
        
        // Terminate after the defined real-world duration.
        if (elapsed.count() / 60.0 >= duration_minutes_) break;

        {
            std::lock_guard<std::mutex> lock(print_mutex);
            std::cout << "\r[Simulating] " << std::fixed << std::setprecision(1) 
                      << elapsed.count() << "s / " << (duration_minutes_ * 60) << "s" << std::flush;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Signal all threads to cease operations and synchronize.
    running = false; 
    for (auto& t : threads) { if (t.joinable()) t.join(); }
    
    std::cout << "\nSimulation reached target duration. Processing stats..." << std::endl;
}

void Simulator::worker_thread(std::shared_ptr<Aircraft> aircraft, std::atomic<bool>& running, double sim_dt_hours, int tick_ms) {
    while (running) {
        auto start = std::chrono::steady_clock::now();

        aircraft->update(sim_dt_hours);

        // Maintain simulation pacing by sleeping for the remainder of the tick.
        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        if (elapsed.count() < tick_ms) {
            std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms) - elapsed);
        }
    }
}

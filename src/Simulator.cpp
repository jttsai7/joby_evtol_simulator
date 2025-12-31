#include "Simulator.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <map>
#include <algorithm>

// Mapping: 1s real-world = 1m simulation. 10ms tick ensures high resolution.
static constexpr int TICK_MS = 10; 
static constexpr double SIM_SPEEDUP = 60.0;
static constexpr double SIM_DT_HOURS = (TICK_MS / 1000.0) * SIM_SPEEDUP / 3600.0;

// Mutex to prevent console log interleaving from multiple aircraft threads.
static std::mutex print_mutex;

Simulator::Simulator(int num_aircraft, int num_chargers, double duration_minutes, TimingMode mode)
    : num_aircraft_(num_aircraft), duration_minutes_(duration_minutes), mode_(mode) // Initialize mode
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
        threads.emplace_back(worker_thread, aircraft, std::ref(running), SIM_DT_HOURS, TICK_MS, mode_);
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
    
    // Final reporting phase after all threads have synchronized
    std::cout << "\n\nSimulation Target Reached. Generating Final Report..." << std::endl;
    generate_report();
}

void Simulator::generate_report() const {
    
    // Intermediate structure to store aggregated metrics 
    // for each manufacturer group.
    struct GroupStats {
        AircraftStats total;
        int vehicle_count = 0;
        int max_faults = 0;
        uint64_t total_ticks = 0; // For system audit
    };

    std::map<CompanyType, GroupStats> report_map;

    // --- Part 1: Individual Vehicle Final States ---
    // Useful for identifying outliers and verifying state-machine transitions
    const int ind_w = 11;
    std::cout << "\n--- Individual Vehicle Final States ---" << std::endl;
    std::cout << std::left << std::setw(6)  << "ID" 
              << std::setw(ind_w) << "Type" 
              << std::setw(ind_w) << "Flight(h)" 
              << std::setw(ind_w) << "Wait(h)"    
              << std::setw(ind_w) << "Charge(h)" 
              << std::setw(ind_w + 4) << "Battery" 
              << std::setw(ind_w) << "Ticks"      
              << std::endl;
    std::cout << std::string(ind_w * 6 + 10, '-') << std::endl;

    for (size_t i = 0; i < fleet_.size(); ++i) {
        const auto& a = fleet_[i];
        const auto& s = a->get_stats();
        
        std::cout << std::left << std::setw(6)  << (i + 1)
                  << std::setw(ind_w) << a->get_name()
                  << std::fixed << std::setprecision(2)
                  << std::setw(ind_w) << s.flight_time_hours
                  << std::setw(ind_w) << s.wait_time_hours    
                  << std::setw(ind_w) << s.charge_time_hours  
                  << std::fixed << std::setprecision(1) << std::setw(5) << a->get_battery_level() << " kWh    "
                  << std::setw(ind_w) << s.completed_ticks 
                  << std::endl;

        // Aggregate statistics
        auto type = a->get_type();
        report_map[type].total.flight_time_hours += s.flight_time_hours;
        report_map[type].total.charge_time_hours += s.charge_time_hours;
        report_map[type].total.wait_time_hours   += s.wait_time_hours;
        report_map[type].total.passenger_miles   += s.passenger_miles;
        report_map[type].total_ticks             += s.completed_ticks;
        
        report_map[type].max_faults = std::max(report_map[type].max_faults, s.fault_count);
        report_map[type].vehicle_count++;
    }

    // --- Part 2: Manufacturer Summary Report ---
    // Final high-level aggregation with fleet averages and KPIs
    const int col_w = 14;
    const std::string separator(col_w * 7 + 8, '=');

    std::cout << "\n" << separator << std::endl;
    std::cout << std::left << std::setw(col_w) << "Vehicle Type" 
              << std::setw(6)     << "Qty" 
              << std::setw(col_w) << "Avg Flight(h)" 
              << std::setw(col_w) << "Avg Wait(h)"    
              << std::setw(col_w) << "Avg Charge(h)" 
              << std::setw(col_w) << "Max Faults" 
              << std::setw(col_w) << "Total Pax-Mi" 
              << std::setw(col_w) << "Avg Ticks"     // Final column
              << std::endl;
    std::cout << std::string(col_w * 7 + 8, '-') << std::endl;

    for (const auto& [type, data] : report_map) {
        if (data.vehicle_count == 0) continue;

        double n = static_cast<double>(data.vehicle_count);
        std::cout << std::left << std::setw(col_w) << AircraftConfig::GetConfig(type).name
                  << std::setw(6)     << data.vehicle_count
                  << std::fixed << std::setprecision(3)
                  << std::setw(col_w) << (data.total.flight_time_hours / n)
                  << std::setw(col_w) << (data.total.wait_time_hours / n)   
                  << std::setw(col_w) << (data.total.charge_time_hours / n) 
                  << std::setw(col_w) << data.max_faults
                  << std::fixed << std::setprecision(1) << std::setw(col_w) << data.total.passenger_miles
                  << std::fixed << std::setprecision(0) << std::setw(col_w) << (static_cast<double>(data.total_ticks) / n)
                  << std::endl;
    }
    std::cout << separator << "\n" << std::endl;
}

void Simulator::worker_thread(std::shared_ptr<Aircraft> aircraft, 
                              std::atomic<bool>& running, 
                              double sim_dt_hours, 
                              int tick_ms,
                              TimingMode mode) {
    
    // Last wake time is only needed for COMPENSATED mode
    auto last_wake_time = std::chrono::steady_clock::now();

    while (running) {
        auto start = std::chrono::steady_clock::now();
        double active_dt = sim_dt_hours;

        // COMPENSATED mode - Compensate for OS scheduling jitter by calculating 
        // actual elapsed time since the last update
        if (mode == TimingMode::COMPENSATED) {
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = now - last_wake_time;
            
            active_dt = (diff.count() * SIM_SPEEDUP) / 3600.0;
            last_wake_time = now;
        }

        // Execute physics update
        aircraft->update(active_dt);

        // FIXED mode - maintain simulation pacing by sleeping for the remainder of the tick
        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start); 
        if (elapsed.count() < tick_ms) {
            std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms) - elapsed);
        }
    }
}


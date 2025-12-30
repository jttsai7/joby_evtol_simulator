#include <gtest/gtest.h>
#include <memory>
#include "Aircraft.h"
#include "ChargerPool.h"

class AircraftTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Provide a pool with ample capacity for baseline physics tests
        default_pool = std::make_shared<ChargerPool>(10);
    }
    std::shared_ptr<ChargerPool> default_pool;
};

// --- Scenario 1: Basic Physics ---
TEST_F(AircraftTest, AlphaPhysicsLogic) {
    Aircraft alpha(CompanyType::Alpha, default_pool);
    // Alpha: 320kWh, 120mph, 1.6kWh/mi -> Power 192kW -> Endurance 1.66h
    alpha.update(1.0);

    EXPECT_DOUBLE_EQ(alpha.get_stats().flight_time_hours, 1.0);
    EXPECT_NEAR(alpha.get_battery_level(), 128.0, 1e-3);
    // 4 pax
    EXPECT_DOUBLE_EQ(alpha.get_stats().passenger_miles, 480.0);
}

// --- Scenario 2: Instant State Transition ---
// Verifies that 'Waiting -> Charging' transition is seamless and doesn't lose time.
TEST_F(AircraftTest, InstantChargingTransition) {
    Aircraft beta(CompanyType::Beta, default_pool);
    
    // Beta: 100 mph, 100 kWh, 1.5 kWh/mi.
    // Power = 150 kW. Max Endurance = 100/150 = 0.6666h. 
    // Updating 0.7h should force it into the Charging state within the same tick.
    beta.update(0.7);

    EXPECT_EQ(beta.get_state(), AircraftState::Charging);
    EXPECT_NEAR(beta.get_stats().flight_time_hours, 0.6666, 1e-3);
    EXPECT_NEAR(beta.get_stats().charge_time_hours, 0.0333, 1e-3);
    EXPECT_DOUBLE_EQ(beta.get_stats().wait_time_hours, 0.0);
}

// --- Scenario 3: Resource Contention ---
TEST_F(AircraftTest, ResourceContentionLogic) {
    // Inject a pool with ZERO capacity to force the aircraft to wait
    auto full_pool = std::make_shared<ChargerPool>(0);
    Aircraft delta(CompanyType::Delta, full_pool);

    // Delta Endurance: approx 1.666 hours.
    delta.update(2.0);

    // Aircraft must remain in Waiting because no chargers are available
    EXPECT_EQ(delta.get_state(), AircraftState::Waiting);
    EXPECT_EQ(delta.get_stats().charge_time_hours, 0.0);
    EXPECT_GT(delta.get_stats().wait_time_hours, 0.3); // Wait time = 2.0 - 1.666
}

// --- Scenario 4: Full Cycle Integration ---
TEST_F(AircraftTest, FullCycleIntegration) {
    Aircraft charlie(CompanyType::Charlie, default_pool);
    
    // Charlie Specs:
    // Cap: 220 kWh, Power: 352 kW (160mph * 2.2kWh/mi)
    // Endurance: 220 / 352 = 0.625h
    // Charge Time: 0.8h
    
    // Step 1: Deplete battery and enter charging
    // Advance 0.7h -> Fly 0.625h (depleted), Charge 0.075h
    charlie.update(0.7); 
    EXPECT_EQ(charlie.get_state(), AircraftState::Charging);

    // Step 2: Finish charging and take off
    // Remaining charge time needed: 0.8 - 0.075 = 0.725h
    // Advance 1.0h -> Charge 0.725h (full), Fly 0.275h
    charlie.update(1.0);

    // Step 3: Verify State
    EXPECT_EQ(charlie.get_state(), AircraftState::Flying);
    
    // Step 4: Verify Precise Battery Level
    // Consumption: 0.275h * 352kW = 96.8 kWh
    // Remaining: 220 - 96.8 = 123.2 kWh
    EXPECT_NEAR(charlie.get_battery_level(), 123.2, 1e-3);
    
    // Verify Total Passenger Miles
    // Total Flight Time: 0.625 (Part 1) + 0.275 (Part 2) = 0.9h
    // Miles: 0.9h * 160mph * 3pax = 432 miles
    EXPECT_NEAR(charlie.get_stats().passenger_miles, 432.0, 1e-3);
}

// --- Scenario 5: Micro-stepping Consistency ---
// Proves that update(1.0) yields the same result as 10,000 calls of update(0.0001)
// Demonstrates the robustness of the time-integration logic.
TEST_F(AircraftTest, ConsistencyCheck) {
    Aircraft a1(CompanyType::Alpha, default_pool);
    Aircraft a2(CompanyType::Alpha, default_pool);
    
    // a1: Run 1.0 hour in a single large step
    a1.update(1.0);
    
    // a2: Run 10,000 small steps of 0.0001 hour each
    double step = 0.0001;
    for(int i=0; i<10000; ++i) {
        a2.update(step);
    }
    
    // The results should be mathematically identical (within float epsilon)
    EXPECT_NEAR(a1.get_battery_level(), a2.get_battery_level(), 1e-3);
    EXPECT_NEAR(a1.get_stats().flight_time_hours, a2.get_stats().flight_time_hours, 1e-3);
}

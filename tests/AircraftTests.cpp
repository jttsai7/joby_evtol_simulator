#include <gtest/gtest.h>
#include "Aircraft.h"

// Scenario: Verify Alpha Company basic physics.
// Alpha: 120 mph, 1.6 kWh/mi, 4 pax.
TEST(AircraftPhysicsTest, AlphaLogic) {
    Aircraft alpha(CompanyType::Alpha);

    // Fly for 1.0 hour
    alpha.update(1.0);

    // Check Flight Time
    EXPECT_DOUBLE_EQ(alpha.get_flight_time_hours(), 1.0);
    
    // Check Battery: 320 - (120 * 1.6) = 128 kWh
    EXPECT_NEAR(alpha.get_battery_level(), 128.0, 1e-3);

    // Check Pax Miles: 120 * 4 = 480
    EXPECT_DOUBLE_EQ(alpha.get_passenger_miles(), 480.0);
}

// Scenario: Verify Precision Logic (Mid-step depletion).
// Beta: 100 mph, 100 kWh, 1.5 kWh/mi.
// Power = 150 kW. Max Endurance = 100/150 = 0.6667 hours.
TEST(AircraftPhysicsTest, PartialStepDepletion) {
    Aircraft beta(CompanyType::Beta);

    // 1.0 hour simulated time.
    // Fly for ~0.6667 hours.
    // The remaining ~0.3333 hours MUST be counted as Waiting.
    beta.update(1.0);

    // 1. State check
    EXPECT_EQ(beta.get_state(), AircraftState::Waiting);
    EXPECT_DOUBLE_EQ(beta.get_battery_level(), 0.0);

    // 2. Flight time check (Should actally fly ~0.6667h, NOT 1.0h)
    EXPECT_NEAR(beta.get_flight_time_hours(), 0.6666, 1e-3);

    // 3. Wait time check
    // 1.0 - 0.6666 = 0.3333
    EXPECT_NEAR(beta.get_wait_time_hours(), 0.3333, 1e-3);
}

// Scenario: Charging Logic
TEST(AircraftPhysicsTest, ChargingCycle) {
    Aircraft charlie(CompanyType::Charlie);
    // Drain it
    charlie.update(2.0); 
    EXPECT_EQ(charlie.get_state(), AircraftState::Waiting);

    // Start Charging
    charlie.start_charging();
    EXPECT_EQ(charlie.get_state(), AircraftState::Charging);

    // Charlie takes 0.8h to charge. Let's update 0.4h.
    charlie.update(0.4);
    // Should be 50% charged (110 kWh)
    EXPECT_NEAR(charlie.get_battery_level(), 110.0, 1e-1);

    // Finish charging
    charlie.update(0.5); 
    // Should transition back to Flying automatically
    EXPECT_EQ(charlie.get_state(), AircraftState::Flying);
}

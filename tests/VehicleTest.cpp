#include <gtest/gtest.h>
#include "Vehicle.h"

namespace JobyTests {

    // Verifies that flying for a set duration drains the correct amount of energy
    TEST(VehicleTest, FlightConsumption) {
        // Charlie: 160 mph, 2.2 kWh/mile, 220 kWh Capacity
        Joby::Vehicle v(Joby::CompanyType::Charlie, 101);
        
        // 30 mins = 80 miles flown = 176 kWh energy consumed
        v.processFlight(30.0);
        
        // Remaining battery: 220 - 176 = 44 kWh
        EXPECT_NEAR(v.getBatteryLevel(), 44.0, 0.01);
        EXPECT_NEAR(v.getTotalDistance(), 80.0, 0.01);
    }

    // Verifies that charging for a set duration replenishes energy linearly
    TEST(VehicleTest, ChargeLogic) {
        // Beta: 100 kWh Capacity, 0.2h (12 mins) to charge fully
        Joby::Vehicle v(Joby::CompanyType::Beta, 102);
        
        // Drain the battery completely first
        v.processFlight(1000); 
        
        // Charge for 6 mins (half the required time) -> should gain 50 kWh
        v.processCharging(6.0);
        EXPECT_NEAR(v.getBatteryLevel(), 50.0, 0.01);
        EXPECT_EQ(v.getState(), Joby::VehicleState::Charging);
    }

} // namespace JobyTests

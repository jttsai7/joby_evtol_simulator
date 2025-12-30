#pragma once

/**
 * Aggregate structure for Key Performance Indicators (KPIs).
 * Designed for accumulation throughout the simulation duration.
 */
struct AircraftStats {
    double flight_time_hours = 0.0;
    double charge_time_hours = 0.0;
    double wait_time_hours = 0.0;
    double passenger_miles = 0.0;
    int fault_count = 0;
};

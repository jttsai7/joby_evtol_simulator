#pragma once
#include <semaphore>

/**
 * Manages charging station availability.
 * Uses a counting semaphore to handle concurrent access from 20 aircraft threads.
 */
class ChargerPool {
public:
    // Initialized with 3 chargers.
    explicit ChargerPool(int total_chargers = 3) : sem_(total_chargers) {}

    // Non-blocking attempt to acquire a charger. 
    // This allows the Aircraft state machine to transition without stalling the thread.
    bool try_acquire() {
        return sem_.try_acquire();
    }

    // Signals that a charger is free for the next aircraft in the system queue.
    void release() {
        sem_.release();
    }

private:
    // Max capacity set to 10 as a safe upper bound for the semaphore template.
    std::counting_semaphore<10> sem_;
};

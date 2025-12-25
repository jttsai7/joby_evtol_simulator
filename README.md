# Joby eVTOL Simulator

A high-performance C++ simulation of eVTOL aircraft operations. This project simulates flight logic, battery consumption, and charging station resource contention in a multi-threaded environment.

### Phase 2: Core Physics & Specifications (Current)
This update establishes the simulation engine's foundation:
* **Vehicle Specifications:** Centralized, thread-safe configuration for 5 aircraft types (Alpha-Echo) using `VehicleConfig`.
* **Physics Engine:** Implemented discrete time-step logic for flight energy consumption and linear battery charging.
* **Verification:** Integrated **Google Test** suite to validate physics formulas and specification integrity.

### Phase 1: Infrastructure & Scaffolding
* **Build System:** Established a robust CMake build environment supporting C++17.
* **Project Structure:** Designed a clean separation of concerns with `src`, `include`, and `tests` directories.

## Requirements

* CMake 3.14+
* C++17 compliant compiler (GCC/Clang/MSVC)

## Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

Run Simulation:
```bash
./evtol_sim
```

Run Unit Tests:
```bash
./test_runner
# or
ctest --verbose
```

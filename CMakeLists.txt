cmake_minimum_required(VERSION 3.14)

project(JobyEvtolSimulator VERSION 1.0 LANGUAGES CXX)

# Fix for CMP0135 (FetchContent timestamp policy)
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---

# Download GoogleTest (Infrastructure setup only)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Prevent GTest from overriding compiler flags
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Find threading library
find_package(Threads REQUIRED)

# --- Main Artifacts ---

add_executable(evtol_sim src/main.cpp src/Vehicle.cpp)
# Use private include scope
target_include_directories(evtol_sim PRIVATE include)
target_link_libraries(evtol_sim PRIVATE Threads::Threads)

# --- Test Infrastructure ---

# Enable CTest runner
enable_testing()

# Define the test runner executable, linking against the vehicle logic
add_executable(test_runner tests/VehicleTest.cpp src/Vehicle.cpp)
target_include_directories(test_runner PRIVATE include)
target_link_libraries(test_runner PRIVATE GTest::gtest_main Threads::Threads)

# Automatically discover tests defined in the source code
include(GoogleTest)
gtest_discover_tests(test_runner)

cmake_minimum_required(VERSION 3.14)
project(Joby_eVTOL_Simulation VERSION 1.0 LANGUAGES CXX)

# Use C++20 for semaphores and modern features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for clangd/VSCode intellisense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- 1. Core Library ---
# Separating logic into a library
add_library(evtol_core 
    src/Aircraft.cpp
)

target_include_directories(evtol_core PUBLIC include)

# Threading support (pthread on Linux)
find_package(Threads REQUIRED)
target_link_libraries(evtol_core PRIVATE Threads::Threads)

# --- 2. Main Executable ---
add_executable(evtol_sim src/main.cpp)
target_link_libraries(evtol_sim PRIVATE evtol_core)

# --- 3. Testing Infrastructure (GoogleTest) ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  # Fixes CMP0135 warning. Updates extracted timestamps to ensure correct rebuilds.
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Windows compatibility: Prevent GTest from overriding CRT settings.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()
add_subdirectory(tests)

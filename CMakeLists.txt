cmake_minimum_required(VERSION 3.14)

project(eVTOL_Simulation VERSION 1.0 LANGUAGES CXX)

# Use C++20 for semaphores and modern features. 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for clangd/VSCode intellisense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Threading support (pthread on Linux)
find_package(Threads REQUIRED)

# --- Source Management ---
add_executable(evtol_sim src/main.cpp)

# Add the project-level 'include' directory to allow direct header inclusion
target_include_directories(evtol_sim PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link threading library
target_link_libraries(evtol_sim PRIVATE Threads::Threads)

# --- Testing Setup (Placeholder) ---
# We will enable testing in Phase 2
# enable_testing()
# add_subdirectory(tests)

add_executable(unit_tests AircraftTests.cpp)

# Link against our Core Lib and GTest main
target_link_libraries(unit_tests 
    PRIVATE 
    evtol_core 
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

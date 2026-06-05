cmake_minimum_required(VERSION 3.16)
project(minical LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---- dependencies ----
include(cmake/SetupDependencies.cmake)
setup_dependencies()

# ---- cal library ----
add_library(cal_lib STATIC
    src/cal_options.cpp
    src/flag_descriptor.cpp
    src/flag_table.cpp
    src/option_parser.cpp
    src/month_calendar.cpp
    src/calendar.cpp
)
target_include_directories(cal_lib PUBLIC src)

# ---- main executable ----
add_executable(minical src/main.cpp)
target_link_libraries(minical PRIVATE cal_lib)

# ---- tests ----
enable_testing()

add_executable(cal_test
    test/test_month_calendar.cpp
    test/test_option_parser.cpp
    test/test_cal_options.cpp
    test/test_integration.cpp
)
target_link_libraries(cal_test PRIVATE cal_lib gtest gtest_main)
target_include_directories(cal_test PRIVATE src)

include(GoogleTest)
gtest_discover_tests(cal_test)

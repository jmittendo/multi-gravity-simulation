cmake_minimum_required(VERSION 3.20)

project(GravitySimulation CXX)

find_package(OpenMP)

add_executable(gravity-simulation
    source/config.cpp
    source/constants.cpp
    source/error_dict.cpp
    source/main.cpp
    source/particle_system.cpp
    source/particle.cpp
    source/unit_system.cpp
    source/util.cpp
    source/vector2d.cpp
)

set_property(TARGET gravity-simulation PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

# For some reason this is required on my machine
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS})

target_compile_features(gravity-simulation PRIVATE cxx_std_23)
target_compile_options(gravity-simulation PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CONFIG:Release>:-O3 -march=native>
)

if(OpenMP_FOUND)
    target_link_libraries(gravity-simulation PRIVATE OpenMP::OpenMP_CXX)
else()
    message(WARNING "OpenMP not found, continuing without it...")
endif()

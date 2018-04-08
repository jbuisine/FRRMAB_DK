cmake_minimum_required (VERSION 2.8)
project (FRRMAB_DK)

include_directories(moead)

add_executable(FRRMAB-DK_UBQP FRRMAB-DK-ubqp.cpp)
add_executable(FRRMAB_UBQP FRRMAB-ubqp.cpp)

add_executable(STATS_GENERATOR_FRRMAB_DK statsDKGenerator.cpp)
add_executable(STATS_GENERATOR_FRRMAB statsGenerator.cpp)


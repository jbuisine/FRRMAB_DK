cmake_minimum_required (VERSION 2.8)
project (FRRMAB_DK)

include_directories(moead)

add_executable(FRRMAB-DK_UBQP FRRMAB-DK-ubqp.cpp)
add_executable(FRRMAB_UBQP FRRMAB-ubqp.cpp)

add_executable(FRRMAB-DK_QAP FRRMAB-DK-qap.cpp)
add_executable(FRRMAB_QAP FRRMAB-qap.cpp)



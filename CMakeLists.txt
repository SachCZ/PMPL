cmake_minimum_required(VERSION 3.15)
project(PMPL)

set(CMAKE_CXX_STANDARD 14)

add_library(Particles particles.cpp)
target_compile_options(Particles PUBLIC $<$<CONFIG:RELEASE>:-O3>)

add_executable(Porous porous.cpp)
target_compile_options(Porous PUBLIC $<$<CONFIG:RELEASE>:-O3>)

add_executable(Carlo carlo.cpp)
target_compile_options(Carlo PUBLIC $<$<CONFIG:RELEASE>:-O3>)

add_executable(Solar solar.cpp)
target_link_libraries(Solar Particles)
target_compile_options(Solar PUBLIC $<$<CONFIG:RELEASE>:-O3>)

add_executable(Collisionless collisionless.cpp)
target_link_libraries(Collisionless Particles)
target_compile_options(Collisionless PUBLIC $<$<CONFIG:RELEASE>:-O3>)

add_executable(Collisional collisional.cpp)
target_link_libraries(Collisional Particles)
target_compile_options(Collisional PUBLIC $<$<CONFIG:RELEASE>:-O3>)

add_executable(Boris boris.cpp)
target_link_libraries(Boris Particles)
target_compile_options(Boris PUBLIC $<$<CONFIG:RELEASE>:-O3>)
cmake_minimum_required(VERSION 3.12)
project(test_package LANGUAGES CXX)

find_package(RPP REQUIRED CONFIG)

add_executable(${PROJECT_NAME} ../basic/basic.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE RPP::rpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

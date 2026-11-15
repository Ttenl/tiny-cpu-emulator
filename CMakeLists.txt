cmake_minimum_required(VERSION 3.16)
project(tiny_cpu_emulator LANGUAGES CXX)

add_executable(tiny_cpu_emulator main.cpp)
target_compile_features(tiny_cpu_emulator PRIVATE cxx_std_17)
set_target_properties(tiny_cpu_emulator PROPERTIES CXX_EXTENSIONS OFF)

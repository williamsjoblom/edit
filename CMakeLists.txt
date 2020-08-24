cmake_minimum_required(VERSION 3.1...3.16)
project(edit VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(edit "")

target_sources(edit PRIVATE
  main.cc
  term.cc
  frame.cc
  buffer.cc)

target_include_directories(edit SYSTEM PRIVATE $ENV{INCLUDE})
set(CMAKE_BUILD_TYPE Debug)

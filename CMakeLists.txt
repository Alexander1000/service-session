cmake_minimum_required(VERSION 2.6)

project(service-session C CXX)

# compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
    third_party/protobuf/src
)

# add the executable
add_executable(service-session server.cc)

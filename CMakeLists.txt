cmake_minimum_required(VERSION 2.6)

# compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# add the executable
add_executable(service-session server.cc)

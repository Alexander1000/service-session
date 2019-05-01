cmake_minimum_required(VERSION 2.6)

project(service-session C CXX)

# compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
    third_party/protobuf/src
    third_party/grpc/include
)

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++_unsecure)

# add the executable
add_executable(service-session server.cc)

target_link_libraries(service-session
    ${_PROTOBUF_LIBPROTOBUF}
    ${_GRPC_GRPCPP_UNSECURE}
)
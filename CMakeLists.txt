cmake_minimum_required(VERSION 2.6)

project(service-session C CXX)

# compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
    third_party/grpc/include
    third_party/protobuf/src
    third_party/cares
    third_party/zlib
)

#set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
#set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++_unsecure)

set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/service.pb.cc")
# set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/service.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.cc")
# set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.h")


# add the executable
add_executable(
    service-session "server.cc"
    ${hw_proto_srcs}
    ${hw_grpc_srcs}
)

#target_link_libraries(service-session
#    ${_GRPC_GRPCPP_UNSECURE}
#    ${_PROTOBUF_LIBPROTOBUF}
#)
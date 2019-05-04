cmake_minimum_required(VERSION 2.6)

project(service-session C CXX)

# compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
    third_party/grpc/include
    third_party/protobuf/src
)

set(GRPC_LIB_DIR /usr/local/lib)

find_library(LIBPROTOBUF libprotobuf.a PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
message(STATUS "Found protobuf libraries: ${LIBPROTOBUF}")

# find_library(LIBARES ares PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBBORINGSSL boringssl PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGPR gpr PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPC grpc PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
find_library(LIBGRPCPP grpc++ PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPC_CRONET grpc_cronet PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPCPP_CORE_STATS grpc++_core_stats PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPCPP_CRONET grpc++_cronet PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPCPP_ERROR_DETAILS grpc++_error_details PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPC_PLUGIN_SUPPORT grpc_plugin_support PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPCPP_REFLECTION grpc++_reflection PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPC_UNSECURE grpc_unsecure PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPCPP_UNSECURE grpc++_unsecure PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)

message(STATUS "Found grpc libraries: ${LIBGRPCPP}")


# add the executable
add_executable(
    service-session "server.cc"
    service.pb.cc
    service.grpc.pb.cc
)

target_link_libraries(service-session
    ${LIBGRPCPP}
    ${LIBPROTOBUF}
)

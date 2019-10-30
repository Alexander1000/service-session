cmake_minimum_required(VERSION 3.7)

project(service-session C CXX)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_STANDARD 17)
elseif(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_STANDARD 14)
elseif(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_STANDARD 11)
else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++11 or C++14 or C++17 support.")
endif()

# compiler options
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GRPC_LIB_DIR /usr/local/lib)

find_library(LIBPROTOBUF libprotobuf.a PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
message(STATUS "Found protobuf libraries: ${LIBPROTOBUF}")

# find_library(LIBARES ares PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBBORINGSSL boringssl PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGPR gpr PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPC grpc PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPCPP grpc++ PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPC_CRONET grpc_cronet PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPCPP_CORE_STATS grpc++_core_stats PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPCPP_CRONET grpc++_cronet PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPCPP_ERROR_DETAILS grpc++_error_details PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPC_PLUGIN_SUPPORT grpc_plugin_support PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPCPP_REFLECTION grpc++_reflection PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
# find_library(LIBGRPC_UNSECURE grpc_unsecure PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)

find_library(LIBGRPCPP_UNSECURE grpc++_unsecure PATHS ${GRPC_LIB_DIR} NO_DEFAULT_PATH)
message(STATUS "Found grpc libraries: ${LIBGRPCPP_UNSECURE}")

find_library(TARANTOOL_C libtarantool.a)
message(STATUS "Found libtarantool library: ${TARANTOOL_C}")

find_library(MSGPACK libmsgpuck.a)
message(STATUS "Found libmsgpuck: ${MSGPACK}")

find_library(JSON_STREAM_ANALYZER libjson-stream-analyzer.a)
message(STATUS "Found json-stream-analyzer library: ${JSON_STREAM_ANALYZER}")

find_library(IO_BUFFER libio-buffer.a)
message(STATUS "Found io-buffer library: ${IO_BUFFER}")

set(SERVICE_SESSION_SOURCES
    service.pb.cc
    service.grpc.pb.cc
    storage.cc
    session_data.cc
)

# add the executable
add_executable(
    ${PROJECT_NAME} "server.cc"
    ${SERVICE_SESSION_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    ${LIBGRPCPP_UNSECURE}
    ${LIBPROTOBUF}
    ${TARANTOOL_C}
    ${MSGPACK}
    ${JSON_STREAM_ANALYZER}
    ${IO_BUFFER}
)

build:
	rm -f ./bin/*.o \
    && g++ \
        -std=c++14 \
        -stdlib=libc++ \
        -O3 \
        -I ~/cpp/grpc/src/cpp/server \
        -I ~/cpp/grpc/third_party/protobuf/src \
        -I ~/cpp/grpc/third_party/libcxx/src \
        -Wall \
        -v \
        -dynamiclib \
        ./server.cc \
        -o ./bin/server.o \
    && g++ ./bin/server.o -o ./bin/server

proto.grpc:
	protoc -I ./proto/ --grpc_out=. --plugin=protoc-gen-grpc=~/cpp/grpc/bins/opt/grpc_cpp_plugin ./proto/service.proto

proto.message:
	protoc -I ./proto --cpp_out=. ./proto/service.proto

build:
	rm -f ./bin/*.o \
    && g++ -Wall ./server.cc -o ./bin/server.o \
    && g++ ./bin/server.o -o ./bin/server

proto.grpc:
	protoc -I ./proto/ --grpc_out=. --plugin=protoc-gen-grpc=~/cpp/grpc/bins/opt/grpc_cpp_plugin ./proto/service.proto

proto.message:
	protoc -I ./proto --cpp_out=. ./proto/service.proto

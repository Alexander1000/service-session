build:
	echo "todo: build"

proto.grpc:
	protoc -I ./proto/ --grpc_out=. --plugin=protoc-gen-grpc=~/cpp/grpc/bins/opt/grpc_cpp_plugin ./proto/service.proto

proto.message:
	protoc -I ./proto --cpp_out=. ./proto/service.proto

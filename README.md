# service-session

How build project:
```shell
git submodule update
cmake .
make
```

Generate c++ code:
```shell
protoc -I ./proto --cpp_out=. ./proto/service.proto
protoc -I ./proto --grpc_out=. --plugin=protoc-gen-grpc=${grpc_path}/bins/opt/grpc_cpp_plugin ./proto/service.proto
```

Run tarantool instance:
```shell
docker run --rm --name session-tarantool -p3301:3301 -d tarantool/tarantool:2.2.0
```

Run service-session instance:
```shell
./service-session --config ./etc/config.local.json
```

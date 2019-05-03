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
```

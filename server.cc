/**
 * Service Session
 */

#include <grpcpp/grpcpp.h>

using grpc::ServerBuilder;

class ServiceSessionServer final : public SessionService::Service {
    ::grpc::Status SessionService::Service::Save(::grpc::ServerContext* context, const ::session::SaveRequest* request, ::session::SaveResponse* response) override {
        (void) context;
        (void) request;
        (void) response;
        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }

    ::grpc::Status SessionService::Service::Get(::grpc::ServerContext* context, const ::session::GetRequest* request, ::session::GetResponse* response) override {
        (void) context;
        (void) request;
        (void) response;
        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
};

int main (int argc, char** argv) {
    std::string server_address("0.0.0.0:50051");
    ServiceSessionServer service;

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();

    return 0;
}

/**
 * Service Session
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "service.grpc.pb.h"

#include "session_data.cc"
#include "storage.cc"

#define TARANTOOL_URI "127.0.0.1:3302"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using session::SaveRequest;
using session::SaveResponse;
using session::GetRequest;
using session::GetResponse;
using session::SessionService;

class ServiceSessionServer final : public SessionService::Service {
public:
    Status Save(ServerContext* context, const SaveRequest* request, SaveResponse* response) override {
        // implement me
        return Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }

    Status Get(ServerContext* context, const GetRequest* request, GetResponse* response) override {
        std::cout << "receive message" << std::endl;
        std::cout << "sessid: " << request->sessid() << std::endl;

        response->set_userid(777);
        response->set_access_token("some-access-token");
        response->set_refresh_token("some-refrech-token");

        Storage storage(TARANTOOL_URI);
        SessionData *sessionData = storage.getBySessId(request->sessid());
        if (sessionData != NULL) {
            response->set_userid(sessionData->userId);
            response->set_access_token(sessionData->accessToken);
            response->set_refresh_token(sessionData->refreshToken);
        }

        return Status::OK;
    }
};

void RunServer() {
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
}

int main (int argc, char** argv) {
    RunServer();

    return 0;
}

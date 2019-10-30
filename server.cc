/**
 * Service Session
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "service.grpc.pb.h"

#include "config.cc"
#include "session_data.cc"
#include "storage.h"

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
    ServiceSessionServer(Config* config) {
        this->config = config;
    }

    Status Save(ServerContext* context, const SaveRequest* request, SaveResponse* response) override {
        Storage* storage = this->getStorage();
        SessionData sessionData(request->sessid(), request->userid(), request->access_token(), request->refresh_token());
        int result = storage->save(&sessionData);
        return Status::OK;
    }

    Status Get(ServerContext* context, const GetRequest* request, GetResponse* response) override {
        std::cout << "receive message" << std::endl;
        std::cout << "sessid: " << request->sessid() << std::endl;

        Storage* storage = this->getStorage();
        SessionData *sessionData = storage->getById(request->sessid());

        if (sessionData != nullptr) {
            response->set_userid(sessionData->userId);
            response->set_access_token(sessionData->accessToken);
            response->set_refresh_token(sessionData->refreshToken);
        } else {
            response->set_userid(777);
            response->set_access_token("some-access-token");
            response->set_refresh_token("some-refrech-token");
        }

        return Status::OK;
    }

    ::grpc::Status Create(::grpc::ServerContext* context, const ::session::CreateRequest* request, ::session::CreateResponse* response) override {
        Storage* storage = this->getStorage();
        char* sessid = new char[65];
        memset(sessid, 0, sizeof(char) * 65);
        int result = storage->create(sessid);
        if (result == 0) {
            response->set_sessid(sessid);
        }
        return Status::OK;
    }

private:
    Storage* getStorage() {
        TarantoolConfig* tConf = this->config->getTarantoolConfig();
        Storage* storage = new Storage(tConf->getUri());
        return storage;
    }

    Config* config;
};

void RunServer(Config* config) {
    std::string server_address(config->getUri());
    ServiceSessionServer service(config);

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
    Config config(argc, argv);

    if (config.isHelp()) {
        std::cout << "How usage it" << std::endl;
        return 0;
    }

    TarantoolConfig* tConf = config.getTarantoolConfig();
    std::cout << "Listen tarantool: " << tConf->getUri() << std::endl;

    RunServer(&config);

    return 0;
}

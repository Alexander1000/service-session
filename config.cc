#include <iostream>
#include <cstring>
#include <map>

#include <io-buffer.h>
#include <json-stream-analyzer.h>

#include "config.h"

TarantoolConfig::TarantoolConfig()
{
    this->host = "127.0.0.1";
    this->port = 3301;
}

char* TarantoolConfig::getUri()
{
    int n_size = std::strlen(this->host.c_str()) + 2 + 6;
    char* uri = new char[n_size];
    memset(uri, 0, sizeof(char) * n_size);
    std::sprintf(uri, "%s:%d", this->host.c_str(), this->port);
    return uri;
}

void TarantoolConfig::parseJsonObject(JsonObject* jsonObject)
{
    if (jsonObject->find("host") != jsonObject->end()) {
        JsonStreamAnalyzer::Element* hostTarantool = jsonObject->at("host");
        if (hostTarantool->getType() == ELEMENT_TYPE_TEXT) {
            std::string *host = (std::string*) hostTarantool->getData();
            this->host = *host;
        }
    }
    if (jsonObject->find("port") != jsonObject->end()) {
        JsonStreamAnalyzer::Element* portTarantool = jsonObject->at("port");
        if (portTarantool->getType() == ELEMENT_TYPE_NUMERIC) {
            std::string *sPort = (std::string*) portTarantool->getData();
            this->port = atoi(sPort->c_str());
        }
    }
}

Config::Config(int argc, char** argv) {
    // initialize defaults
    this->is_help = false;

    this->tarantoolConfig = new TarantoolConfig;

    this->listen = std::string("127.0.0.1");
    this->port = 50051;

    if (argc == 1) {
        // run without params
        return;
    }

    for (int i = 1; i < argc; i++) {
        std::string paramName(argv[i]);

        if (paramName.compare("-h") == 0 || paramName.compare("--help") == 0) {
            this->is_help = true;
            continue;
        }

        if (paramName.compare("-c") == 0 || paramName.compare("--config") == 0) {
            if (i + 1 == argc) {
                // todo: error missed argument value
                continue;
            }
            std::string paramValue(argv[i+1]);
            this->parse_config_file(paramValue);
            i++;
            continue;
        }
    }
}

bool Config::isHelp()
{
    return this->is_help;
}

TarantoolConfig* Config::getTarantoolConfig()
{
    return this->tarantoolConfig;
}

char* Config::getUri() {
    int n_size = strlen(this->listen.c_str()) + 2 + 6;
    char* uri = new char[n_size];
    memset(uri, 0, sizeof(char) * n_size);
    std::sprintf(uri, "%s:%d", this->listen.c_str(), this->port);
    return uri;
}

void Config::parse_config_file(std::string file_name)
{
    // todo: file analyze (json/yml/cfg/ini)

    IOBuffer::IOFileReader fileReader(file_name.c_str());
    IOBuffer::CharStream charStream(&fileReader);
    JsonStreamAnalyzer::Stream stream(&charStream);
    JsonStreamAnalyzer::Decoder decoder(&stream);

    JsonStreamAnalyzer::Element* element = decoder.decode();

    if (element->getType() == ELEMENT_TYPE_OBJECT) {
        JsonObject* obj = (JsonObject*) element->getData();

        if (obj->find("tarantool") != obj->end()) {
            JsonStreamAnalyzer::Element* tarantoolConfig = obj->at("tarantool");
            if (tarantoolConfig->getType() == ELEMENT_TYPE_OBJECT) {
                this->tarantoolConfig->parseJsonObject((JsonObject*) tarantoolConfig->getData());
            }
        }

        if (obj->find("server") != obj->end()) {
            JsonStreamAnalyzer::Element* serverConfig = obj->at("server");
            if (serverConfig->getType() == ELEMENT_TYPE_OBJECT) {
                // parse server object
                JsonObject* serverJsonObj = (JsonObject*) serverConfig->getData();

                if (serverJsonObj->find("listen") != serverJsonObj->end()) {
                    JsonStreamAnalyzer::Element* eListenIp = serverJsonObj->at("listen");
                    if (eListenIp->getType() == ELEMENT_TYPE_TEXT) {
                        std::string* listen = (std::string*) eListenIp->getData();
                        this->listen = *listen;
                    }
                }

                if (serverJsonObj->find("port") != serverJsonObj->end()) {
                    JsonStreamAnalyzer::Element* ePort = serverJsonObj->at("port");
                    if (ePort->getType() == ELEMENT_TYPE_NUMERIC) {
                        std::string *sPort = (std::string*) ePort->getData();
                        this->port = atoi(sPort->c_str());
                    }
                }
            }
        }
    }
}

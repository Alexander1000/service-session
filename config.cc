#include <iostream>
#include <string>
#include <map>

#include <json-stream-analyzer.h>

typedef std::map<std::string, JsonStreamAnalyzer::Element *> JsonObject;

class TarantoolConfig {
public:
    TarantoolConfig() {
        this->host = "127.0.0.1";
        this->port = 3301;
    }

    std::string host;
    int port;

    char* getUri() {
        int n_size = strlen(this->host.c_str()) + 2 + 6;
        char* uri = new char[n_size];
        memset(uri, 0, sizeof(char) * n_size);
        std::sprintf(uri, "%s:%d", this->host.c_str(), this->port);
        return uri;
    }
};

class Config
{
public:
    Config(int argc, char** argv) {
        // initialize defaults
        this->is_help = false;

        this->tarantoolConfig = new TarantoolConfig;

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

    bool isHelp() {
        return this->is_help;
    }

    TarantoolConfig* getTarantoolConfig()
    {
        return this->tarantoolConfig;
    }
private:
    bool is_help;

    TarantoolConfig* tarantoolConfig;

    void parse_config_file(std::string file_name)
    {
        // todo: file analyze (json/yml/cfg/ini)

        JsonStreamAnalyzer::Buffer::IOFileReader fileReader(file_name.c_str());
        JsonStreamAnalyzer::Stream stream(&fileReader);
        JsonStreamAnalyzer::Decoder decoder(&stream);

        JsonStreamAnalyzer::Element* element = decoder.decode();

        if (element->getType() == ELEMENT_TYPE_OBJECT) {
            JsonObject* obj = (JsonObject*) element->getData();
            if (obj->find("tarantool") != obj->end()) {
                JsonStreamAnalyzer::Element* tarantoolConfig = obj->at("tarantool");
                if (tarantoolConfig->getType() == ELEMENT_TYPE_OBJECT) {
                    JsonObject* tarantoolObject = (JsonObject*) tarantoolConfig->getData();
                    if (tarantoolObject->find("host") != tarantoolObject->end()) {
                        JsonStreamAnalyzer::Element* hostTarantool = tarantoolObject->at("host");
                        if (hostTarantool->getType() == ELEMENT_TYPE_TEXT) {
                            std::string *host = (std::string*) hostTarantool->getData();
                            std::cout << "Host: " << *host << std::endl;
                            this->tarantoolConfig->host = *host;
                        }
                    }
                    if (tarantoolObject->find("port") != tarantoolObject->end()) {
                        JsonStreamAnalyzer::Element* portTarantool = tarantoolObject->at("port");
                        if (portTarantool->getType() == ELEMENT_TYPE_NUMERIC) {
                            std::string *sPort = (std::string*) portTarantool->getData();
                            std::cout << "Port: " << *sPort << std::endl;
                            this->tarantoolConfig->port = atoi(sPort->c_str());
                        }
                    }
                }
            }
        }
    }
};

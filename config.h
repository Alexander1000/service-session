#ifndef SERVICE_SESSION_CONFIG_H
#define SERVICE_SESSION_CONFIG_H

#include <string>
#include <map>

#include <json-stream-analyzer.h>

typedef std::map<std::string, JsonStreamAnalyzer::Element *> JsonObject;

class TarantoolConfig
{
public:
    std::string host;
    int port;

    TarantoolConfig();

    char* getUri();

    void parseJsonObject(JsonObject* jsonObject);
};

class Config
{
    bool is_help;

    TarantoolConfig* tarantoolConfig;

    std::string listen;

    int port;

    void parse_config_file(std::string file_name);

public:
    Config(int argc, char **argv);

    bool isHelp();

    TarantoolConfig* getTarantoolConfig();

    char* getUri();
};

#endif //SERVICE_SESSION_CONFIG_H

#include <iostream>
#include <string>

#include <json-stream-analyzer.h>

class Config
{
public:
    Config(int argc, char** argv) {
        // initialize defaults
        this->is_help = false;

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
private:
    bool is_help;

    void parse_config_file(std::string file_name)
    {
        JsonStreamAnalyzer::Buffer::IOFileReader fileReader(file_name.c_str());
        JsonStreamAnalyzer::Stream stream(&fileReader);
        JsonStreamAnalyzer::Decoder decoder(&stream);
    }
};

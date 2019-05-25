#include <iostream>
#include <string>

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
        }
    }

    bool isHelp() {
        return this->is_help;
    }
private:
    bool is_help;
};

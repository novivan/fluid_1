#include "Simulator.h"
#include "Types.h"
#include <iostream>
#include <string>
#include <map>

// Макросы для выбора типов
#define SELECT_TYPE(p, v, vflow, size1, size2) Simulator<p, v, vflow, size1, size2>

int main(int argc, char* argv[]) {
    std::cout << "Starting simulation..." << std::endl;
    std::cout << "Initializing variables..." << std::endl;

    std::map<std::string, std::string> args;
    for (int i = 1; i < argc; i += 2) {
        args[argv[i]] = argv[i + 1];
    }

    std::string source = args["--source"];
    std::string p_type = args["--p-type"];
    std::string v_type = args["--v-type"];
    std::string v_flow_type = args["--v-flow-type"];

    std::cout << "Source: " << source << std::endl;
    std::cout << "p_type: " << p_type << std::endl;
    std::cout << "v_type: " << v_type << std::endl;
    std::cout << "v_flow_type: " << v_flow_type << std::endl;

    if (p_type == "DOUBLE" && v_type == "FIXED(32,16)" && v_flow_type == "FAST_FIXED(33,20)") {
        std::cout << "Creating simulator..." << std::endl;
        Simulator<DOUBLE, FIXED_32_16, FAST_FIXED<33, 20>, 1920, 1080> sim;
        std::cout << "Running simulator..." << std::endl;
        sim.run(source);
    } else {
        std::cerr << "Unsupported type combination\n";
        return 1;
    }

    return 0;
}

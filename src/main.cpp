#include "Simulator.h"
#include "Types.h"
#include <iostream>
#include <string>
#include <map>

int main(int argc, char* argv[]) {
    std::map<std::string, std::string> args;
    for (int i = 1; i < argc; i += 2) {
        args[argv[i]] = argv[i + 1];
    }

    std::string source = args["--source"];
    std::string p_type = args["--p-type"];
    std::string v_type = args["--v-type"];
    std::string v_flow_type = args["--v-flow-type"];

    if (p_type == "DOUBLE" && v_type == "FIXED<32,16>" && v_flow_type == "FAST_FIXED<33,20>") {
        // Создаем экземпляр симулятора напрямую с использованием шаблонных параметров
        Simulator<DOUBLE, FIXED<32, 16>, FAST_FIXED<33, 20>, 1920, 1080> sim;
        sim.run(source);
    } else {
        std::cerr << "Unsupported type combination\n";
        return 1;
    }

    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include "fluid.hpp"
#include "utils/file_reader.hpp"
#include "parameter_selector.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input-file> --p-type=<type> --v-type=<type> --v-flow-type=<type>" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    std::string p_type, v_type, v_flow_type;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("--p-type=") == 0) {
            p_type = arg.substr(9);
        } else if (arg.find("--v-type=") == 0) {
            v_type = arg.substr(9);
        } else if (arg.find("--v-flow-type=") == 0) {
            v_flow_type = arg.substr(14);
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return 1;
        }
    }

    if (p_type.empty() || v_type.empty() || v_flow_type.empty()) {
        std::cerr << "All types (--p-type, --v-type, --v-flow-type) must be specified." << std::endl;
        return 1;
    }

    // Проверка, что все типы были скомпилированы
    if (!isTypeCompiled(p_type)) {
        std::cerr << "Type " << p_type << " was not compiled." << std::endl;
        return 1;
    }
    if (!isTypeCompiled(v_type)) {
        std::cerr << "Type " << v_type << " was not compiled." << std::endl;
        return 1;
    }
    if (!isTypeCompiled(v_flow_type)) {
        std::cerr << "Type " << v_flow_type << " was not compiled." << std::endl;
        return 1;
    }

    // Пример использования типов и размеров
    const size_t N = 36;
    const size_t M = 84;

    using PType = decltype(parseType<float>(p_type));
    using VType = decltype(parseType<float>(v_type));
    using VFlowType = decltype(parseType<float>(v_flow_type));

    ParameterSelector<PType, N, M>::SimulatorType simulator;

    std::vector<std::vector<float>> initial_data;
    float g;
    std::vector<float> rho;

    readInputData<N, M>(input_file, initial_data, g, rho);
    simulator.initialize(initial_data);
    simulator.simulate();
    simulator.render();

    return 0;
}
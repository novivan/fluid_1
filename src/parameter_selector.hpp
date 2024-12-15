#ifndef PARAMETER_SELECTOR_HPP
#define PARAMETER_SELECTOR_HPP

#include <string>
#include <stdexcept>
#include "types.hpp"
#include "fluid.hpp"

template<typename T, size_t N, size_t M>
struct ParameterSelector {
    using SimulatorType = FluidSimulator<T, N, M>;
};

template<typename T>
T parseType(const std::string& type) {
    if (type == "FLOAT") return FLOAT();
    if (type == "DOUBLE") return DOUBLE();
    if (type == "FAST_FIXED(13,7)") return FAST_FIXED(13, 7)();
    if (type == "FIXED(32,5)") return FIXED(32, 5)();
    throw std::invalid_argument("Unknown type: " + type);
}

bool isTypeCompiled(const std::string& type) {
    #ifdef FLOAT
    if (type == "FLOAT") {
        std::cout << "FLOAT is compiled" << std::endl;
        return true;
    }
    #endif
    #ifdef DOUBLE
    if (type == "DOUBLE") {
        std::cout << "DOUBLE is compiled" << std::endl;
        return true;
    }
    #endif
    #ifdef FAST_FIXED(13,7)
    if (type == "FAST_FIXED(13,7)") {
        std::cout << "FAST_FIXED(13,7) is compiled" << std::endl;
        return true;
    }
    #endif
    #ifdef FIXED(32,5)
    if (type == "FIXED(32,5)") {
        std::cout << "FIXED(32,5) is compiled" << std::endl;
        return true;
    }
    #endif
    std::cout << "Type " << type << " is not compiled" << std::endl;
    return false;
}

#endif // PARAMETER_SELECTOR_HPP
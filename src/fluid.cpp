#include "fluid.hpp"
#include "utils/file_reader.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>

void runSimulation() {
    std::vector<std::vector<float>> initial_data;
    float g;
    std::vector<float> rho;

    readInputData<36, 84>("input/input.txt", initial_data, g, rho);

    constexpr size_t N = 36;
    constexpr size_t M = 84;
    FluidSimulator<float, N, M> simulator;
    simulator.initialize(initial_data);

    for (int i = 0; i < 10; ++i) { // Пример 10 итераций
        simulator.update();
        simulator.render();
    }
}
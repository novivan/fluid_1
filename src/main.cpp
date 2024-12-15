#include <iostream>
#include "fluid.hpp"
#include "utils/file_reader.hpp"

int main(int argc, char* argv[]) {
    // Проверка наличия необходимых аргументов командной строки
    if (argc < 2) {
        std::cerr << "Использование: " << argv[0] << " <путь к файлу входных данных>" << std::endl;
        return 1;
    }

    // Считывание параметров из файла
    std::string input_file = argv[1];
    constexpr size_t N = 36;
    constexpr size_t M = 84;
    FluidSimulator<float, N, M> simulator;

    // Инициализация симуляции
    std::vector<std::vector<float>> initial_data;
    float g;
    std::vector<float> rho;
    readInputData<N, M>(input_file, initial_data, g, rho);
    simulator.initialize(initial_data);

    // Запуск основного цикла симуляции
    for (int i = 0; i < 10; ++i) { // Пример 10 итераций
        simulator.update();
        simulator.render();
    }

    return 0;
}
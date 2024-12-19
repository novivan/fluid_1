#include "Simulator.h"

template<typename P, typename V, typename VFLOW, size_t S1, size_t S2>
Simulator<P, V, VFLOW, S1, S2>::Simulator() : UT(0) {
    std::cout << "Initializing simulator..." << std::endl;
    // Инициализация rho и других полей при необходимости
    for (int i = 0; i < 256; ++i) {
        rho[i] = static_cast<P>(0);
    }
    for (size_t i = 0; i < S1; ++i) {
        for (size_t j = 0; j < S2; ++j) {
            p[i][j] = static_cast<P>(0);
            velocity[i][j] = static_cast<V>(0);
            velocity_flow[i][j] = static_cast<VFLOW>(0);
        }
    }
}

template<typename P, typename V, typename VFLOW, size_t S1, size_t S2>
void Simulator<P, V, VFLOW, S1, S2>::run(const std::string& source) {
    std::cout << "Loading from file: " << source << std::endl;
    load_from_file(source);
    std::cout << "Starting simulation..." << std::endl;
    // Основной цикл симуляции
    for (int step = 0; step < 100; ++step) { // Пример 100 итераций
        // Пример симуляции: простое обновление значений
        for (size_t i = 0; i < S1; ++i) {
            for (size_t j = 0; j < S2; ++j) {
                p[i][j] = p[i][j] + static_cast<P>(0.1); // Пример обновления значений
            }
        }
        // Вывод текущего состояния системы
        std::cout << "Step " << step << ":\n";
        for (size_t i = 0; i < S1; ++i) {
            for (size_t j = 0; j < S2; ++j) {
                std::cout << p[i][j] << " ";
            }
            std::cout << "\n";
        }
    }
}

template<typename P, typename V, typename VFLOW, size_t S1, size_t S2>
void Simulator<P, V, VFLOW, S1, S2>::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл");
    }
    size_t width, height;
    file >> width >> height;
    P g;
    file >> g;
    std::cout << "Width: " << width << ", Height: " << height << ", g: " << g << std::endl;
    for (int i = 0; i < 256; ++i) {
        if (!(file >> rho[i])) {
            throw std::runtime_error("Ошибка чтения rho из файла");
        }
    }
    // Чтение начального расположения жидкостей
    int x, y;
    P value;
    while (file >> x >> y >> value) {
        if (x >= S1 || y >= S2) {
            throw std::runtime_error("Координаты выходят за пределы массива");
        }
        p[x][y] = value;
    }
    std::cout << "Finished loading from file." << std::endl;
}

// Явная инстанциация шаблонов (можно добавить необходимые комбинации типов и размеров)
template class Simulator<FIXED_32_16, FLOAT, FLOAT, 1920, 1080>;
template class Simulator<FIXED_16_8, DOUBLE, DOUBLE, 10, 10>;
template class Simulator<DOUBLE, FIXED_32_16, FAST_FIXED<33, 20>, 1920, 1080>;

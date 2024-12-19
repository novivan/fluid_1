#include "Simulator.h"

#define SIMULATOR_TEMPLATE_PARAMS typename P, typename V, typename VFLOW, size_t S1, size_t S2
#define SIMULATOR_TEMPLATE_ARGS P, V, VFLOW, S1, S2

template<SIMULATOR_TEMPLATE_PARAMS>
Simulator<SIMULATOR_TEMPLATE_ARGS>::Simulator() : UT(0) {
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

template<SIMULATOR_TEMPLATE_PARAMS>
void Simulator<SIMULATOR_TEMPLATE_ARGS>::run(const std::string& source) {
    load_from_file(source);
    // Основной цикл симуляции
    for (int step = 0; step < 100; ++step) { // Пример 100 итераций
        // Пример симуляции: простое обновление значений
        for (size_t i = 0; i < S1; ++i) {
            for (size_t j = 0; j < S2; ++j) {
                p[i][j] = p[i][j] + static_cast<P>(0.1); // Пример обновления значений
            }
        }
        // Вывод текущего состояния системы
        for (size_t i = 0; i < S1; ++i) {
            for (size_t j = 0; j < S2; ++j) {
                // ...existing code...
            }
        }
    }
}

template<SIMULATOR_TEMPLATE_PARAMS>
void Simulator<SIMULATOR_TEMPLATE_ARGS>::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл");
    }
    size_t width, height;
    file >> width >> height;
    P g;
    file >> g;
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
}

// Явная инстанциация шаблонов (заменяем алиасы на шаблонные параметры)
template class Simulator<FIXED<32, 16>, FLOAT, FLOAT, 1920, 1080>;
template class Simulator<FIXED<16, 8>, DOUBLE, DOUBLE, 10, 10>;
template class Simulator<DOUBLE, FIXED<32, 16>, FAST_FIXED<33, 20>, 1920, 1080>;

// Явная инстанциация шаблона для используемых типов и размеров
// template class Simulator<DOUBLE, FIXED<32, 16>, FAST_FIXED<33, 20>, 1920, 1080>;

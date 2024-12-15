#ifndef FLUID_HPP
#define FLUID_HPP

#include <array>
#include <iostream>
#include <tuple>
#include <vector>
#include <cassert>
#include <random>

template<typename T, size_t N, size_t M>
class FluidSimulator {
public:
    FluidSimulator();
    void initialize(const std::vector<std::vector<T>>& initial_conditions);
    void update();
    void render() const;

private:
    std::array<std::array<T, M>, N> field;
    std::array<std::array<T, M>, N> velocity;
    T gravity;
    std::mt19937 rnd;

    void apply_forces();
    void propagate_flow();
    void calculate_pressure();
};

template<typename T, size_t N, size_t M>
FluidSimulator<T, N, M>::FluidSimulator() : gravity(9.81), rnd(std::random_device{}()) {
    // Инициализация поля и скорости
}

template<typename T, size_t N, size_t M>
void FluidSimulator<T, N, M>::initialize(const std::vector<std::vector<T>>& initial_conditions) {
    std::cout << "Размер initial_conditions: " << initial_conditions.size() << "x" << initial_conditions[0].size() << std::endl;
    std::cout << "Ожидаемый размер: " << N << "x" << M << std::endl;
    assert(initial_conditions.size() == N && initial_conditions[0].size() == M);
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            field[i][j] = initial_conditions[i][j];
        }
    }
}

template<typename T, size_t N, size_t M>
void FluidSimulator<T, N, M>::update() {
    // Логика обновления состояния жидкости
}

template<typename T, size_t N, size_t M>
void FluidSimulator<T, N, M>::render() const {
    for (const auto& row : field) {
        for (const auto& cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
}

#endif // FLUID_HPP
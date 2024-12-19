#pragma once

#include "Types.h"
#include <array>
#include <tuple>
#include <string>
#include <fstream>
#include <iostream>

template<typename P, typename V, typename VFLOW, size_t S1, size_t S2>
class Simulator {
public:
    Simulator();
    void run(const std::string& source);

private:
    static constexpr std::array<std::pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

    std::array<std::array<P, S2>, S1> p;
    std::array<std::array<V, S2>, S1> velocity;
    std::array<std::array<VFLOW, S2>, S1> velocity_flow;

    P rho[256];
    int UT;

    std::tuple<P, bool, std::pair<int, int>> propagate_flow(int x, int y, P lim);
    void load_from_file(const std::string& filename);
};

template<typename P, typename V, typename VFLOW, size_t S1, size_t S2>
std::tuple<P, bool, std::pair<int, int>> Simulator<P, V, VFLOW, S1, S2>::propagate_flow(int x, int y, P lim) {
    // ...existing code...
    return std::make_tuple(static_cast<P>(0), false, std::make_pair(0, 0));
}

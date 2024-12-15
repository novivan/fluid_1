#ifndef FILE_READER_HPP
#define FILE_READER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

struct Position {
    int x;
    int y;
};

struct FluidData {
    Position position;
    float density;
};

template<size_t N, size_t M>
void readInputData(const std::string& filename, std::vector<std::vector<float> >& initial_data, float& g, std::vector<float>& rho) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filename);
    }

    initial_data.assign(N, std::vector<float>(M, 0.0f)); // Инициализация массива с размерами N и M

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        if (line.find("g = ") != std::string::npos) {
            iss >> key >> g;
        } else if (line.find("rho = ") != std::string::npos) {
            float density;
            iss >> key >> density;
            rho.push_back(density);
        } else if (line.find("initial_positions = [") != std::string::npos) {
            while (std::getline(file, line) && line.find("]") == std::string::npos) {
                std::istringstream pos_iss(line);
                char ignore;
                int x, y;
                std::string type;
                pos_iss >> ignore >> x >> ignore >> y >> ignore >> type >> ignore;
                if (x >= 0 && x < N && y >= 0 && y < M) {
                    initial_data[x][y] = 1.0f; // Пример заполнения значением
                }
            }
        }
    }

    file.close();
}


#endif // FILE_READER_HPP
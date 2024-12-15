#include "file_reader.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

// Не забудьте добавить реализацию шаблона в файл реализации
template void readInputData<36, 84>(const std::string& filename, std::vector<std::vector<float>>& initial_data, float& g, std::vector<float>& rho);
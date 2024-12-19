#pragma once

#include "Fixed.h"

// Определение типов данных
using FLOAT = float;
using DOUBLE = double;
template<size_t N, size_t K> using FIXED = Fixed<N, K>;
template<size_t N, size_t K> using FAST_FIXED = Fixed<N, K>; // Можно изменить на более оптимальные типы при необходимости

// Примеры типов
using FIXED_32_16 = FIXED<32, 16>;
using FIXED_16_8 = FIXED<16, 8>;

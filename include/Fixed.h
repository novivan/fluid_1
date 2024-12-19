#pragma once

#include <cstdint>
#include <type_traits>
#include <limits>
#include <stdexcept>
#include <iostream>

template <size_t N, size_t K>
class Fixed {
    static_assert(N <= 64, "FIXED type exceeds 64 bits");
public:
    using storage_type = typename std::conditional_t<
        (N <= 8), int_fast8_t,
        typename std::conditional_t<
            (N <= 16), int_fast16_t,
            typename std::conditional_t<
                (N <= 32), int_fast32_t,
                int_fast64_t
            >
        >
    >;

    storage_type value;

    Fixed() : value(0) {}
    Fixed(double d) {
        double scaled = d * (1 << K);
        if (scaled > std::numeric_limits<storage_type>::max() || scaled < std::numeric_limits<storage_type>::min()) {
            throw std::overflow_error("Fixed point overflow");
        }
        value = static_cast<storage_type>(scaled);
    }

    Fixed operator+(const Fixed& other) const {
        Fixed result;
        result.value = this->value + other.value;
        return result;
    }

    Fixed operator-(const Fixed& other) const {
        Fixed result;
        result.value = this->value - other.value;
        return result;
    }

    Fixed operator*(const Fixed& other) const {
        Fixed result;
        result.value = (this->value * other.value) >> K;
        return result;
    }

    Fixed operator/(const Fixed& other) const {
        Fixed result;
        result.value = (this->value << K) / other.value;
        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const Fixed& fixed) {
        os << static_cast<double>(fixed.value) / (1 << K);
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Fixed& fixed) {
        double d;
        is >> d;
        fixed = Fixed(d);
        return is;
    }
};

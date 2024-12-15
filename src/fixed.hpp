#ifndef FIXED_HPP
#define FIXED_HPP

#include <cstdint>
#include <iostream>

template<int N, int K>
class Fixed {
public:
    Fixed() : value(0) {}
    Fixed(float f) : value(static_cast<int32_t>(f * (1 << K))) {}
    Fixed(double d) : value(static_cast<int32_t>(d * (1 << K))) {}

    operator float() const { return static_cast<float>(value) / (1 << K); }
    operator double() const { return static_cast<double>(value) / (1 << K); }

    Fixed<N, K>& operator+=(const Fixed<N, K>& other) {
        value += other.value;
        return *this;
    }

    Fixed<N, K>& operator-=(const Fixed<N, K>& other) {
        value -= other.value;
        return *this;
    }

    Fixed<N, K>& operator*=(const Fixed<N, K>& other) {
        value = (static_cast<int64_t>(value) * other.value) >> K;
        return *this;
    }

    Fixed<N, K>& operator/=(const Fixed<N, K>& other) {
        value = (static_cast<int64_t>(value) << K) / other.value;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Fixed<N, K>& fixed) {
        os << static_cast<float>(fixed);
        return os;
    }

private:
    int32_t value;
};

#endif // FIXED_HPP
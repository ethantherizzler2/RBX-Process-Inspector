#pragma once
#include <iostream>
#include <cmath>

struct Vector3 {
    float X, Y, Z;

    constexpr Vector3() : X(0.0f), Y(0.0f), Z(0.0f) {}
    constexpr Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}

    float Length() const {
        return std::sqrt(X * X + Y * Y + Z * Z);
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        return os << "(" << v.X << ", " << v.Y << ", " << v.Z << ")";
    }
};
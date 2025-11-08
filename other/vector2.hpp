#pragma once
#include <cmath>
#include <iostream>

struct Vector3 {
    float X, Y, Z;

    constexpr Vector3() noexcept : X(0.0f), Y(0.0f), Z(0.0f) {}
    constexpr Vector3(float x, float y, float z) noexcept : X(x), Y(y), Z(z) {}

    [[nodiscard]] float Length() const noexcept {
        return std::sqrt(X * X + Y * Y + Z * Z);
    }

    [[nodiscard]] float LengthSquared() const noexcept {
        return X * X + Y * Y + Z * Z;
    }

    [[nodiscard]] Vector3 Normalized() const noexcept {
        float len = Length();
        return (len > 0.0001f) ? Vector3(X / len, Y / len, Z / len) : Vector3();
    }

    // Arithmetic operators
    constexpr Vector3 operator+(const Vector3& other) const noexcept {
        return { X + other.X, Y + other.Y, Z + other.Z };
    }

    constexpr Vector3 operator-(const Vector3& other) const noexcept {
        return { X - other.X, Y - other.Y, Z - other.Z };
    }

    constexpr Vector3 operator*(float scalar) const noexcept {
        return { X * scalar, Y * scalar, Z * scalar };
    }

    constexpr Vector3 operator/(float scalar) const noexcept {
        return (scalar != 0.0f) ? Vector3(X / scalar, Y / scalar, Z / scalar) : Vector3();
    }

    Vector3& operator+=(const Vector3& other) noexcept {
        X += other.X; Y += other.Y; Z += other.Z;
        return *this;
    }

    Vector3& operator-=(const Vector3& other) noexcept {
        X -= other.X; Y -= other.Y; Z -= other.Z;
        return *this;
    }

    Vector3& operator*=(float scalar) noexcept {
        X *= scalar; Y *= scalar; Z *= scalar;
        return *this;
    }

    Vector3& operator/=(float scalar) noexcept {
        if (scalar != 0.0f) { X /= scalar; Y /= scalar; Z /= scalar; }
        return *this;
    }

    [[nodiscard]] float Dot(const Vector3& other) const noexcept {
        return X * other.X + Y * other.Y + Z * other.Z;
    }

    [[nodiscard]] float Distance(const Vector3& other) const noexcept {
        return std::sqrt((X - other.X) * (X - other.X) +
                         (Y - other.Y) * (Y - other.Y) +
                         (Z - other.Z) * (Z - other.Z));
    }

    // output
    friend std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        return os << "(" << v.X << ", " << v.Y << ", " << v.Z << ")";
    }
};

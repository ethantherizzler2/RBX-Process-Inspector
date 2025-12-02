#pragma once
#include "other/vector.hpp"

struct Matrix4 {
    float matrix[16];

    Matrix4() {
        for (int i = 0; i < 16; i++) matrix[i] = 0.0f;
    }

    Vector3 transform_point(const Vector3& point) const {
        return Vector3(
            point.X * matrix[0] + point.Y * matrix[4] + point.Z * matrix[8] + matrix[12],
            point.X * matrix[1] + point.Y * matrix[5] + point.Z * matrix[9] + matrix[13],
            point.X * matrix[2] + point.Y * matrix[6] + point.Z * matrix[10] + matrix[14]
        );
    }
};
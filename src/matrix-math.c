#include "../include/matrix-math.h"

Vec3 MatrixVec3Multiplication(Vec3 vec, int matrixSize, float  matrix[matrixSize][matrixSize]) {
    Vec3 ret = { 0, 0, 0 };

    ret.x += (
        vec.x * matrix[0][0] +
        vec.y * matrix[0][1] +
        vec.z * matrix[0][2]
    );

    ret.y += (
        vec.x * matrix[1][0] +
        vec.y * matrix[1][1] +
        vec.z * matrix[1][2]
    );

    ret.z += (
        vec.x * matrix[2][0] +
        vec.y * matrix[2][1] +
        vec.z * matrix[2][2]
    );

    return ret;
}

Vec4 MatrixVec4Multiplication(Vec4 vec, int matrixSize, float matrix[matrixSize][matrixSize]) {
    Vec4 ret = { 0, 0, 0, 0 };

    ret.x += (
        vec.x * matrix[0][0] +
        vec.y * matrix[0][1] +
        vec.z * matrix[0][2] +
        vec.w * matrix[0][3]
    );

    ret.y += (
        vec.x * matrix[1][0] +
        vec.y * matrix[1][1] +
        vec.z * matrix[1][2] +
        vec.w * matrix[1][3]
    );

    ret.z += (
        vec.x * matrix[2][0] +
        vec.y * matrix[2][1] +
        vec.z * matrix[2][2] +
        vec.w * matrix[2][3]
    );

    ret.w += (
        vec.x * matrix[3][0] +
        vec.y * matrix[3][1] +
        vec.z * matrix[3][2] +
        vec.w * matrix[3][3]
    );

    return ret;
}

Vec3 AddVec3(Vec3 vec1, Vec3 vec2) {
    return (Vec3) {vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z};
}

Vec3 RotateVec3AroundAxis(Vec3 vec, float angle, RotationAxis axis) {
    switch (axis) {
        case X_AXIS:
            return MatrixVec3Multiplication(vec, 3, 
                (float[3][3]){
                    {1, 0, 0},
                    {0, cos(angle), - sin(angle)},
                    {0, sin(angle), cos(angle)},
                }
            );
        case Y_AXIS:
            return MatrixVec3Multiplication(vec, 3, 
                (float[3][3]){
                    {cos(angle), 0, sin(angle)},
                    {0, 1, 0},
                    {- sin(angle), 0, cos(angle)},
                }
            );
        case Z_AXIS:
            return MatrixVec3Multiplication(vec, 3, 
                (float[3][3]){
                    {cos(angle), - sin(angle), 0},
                    {sin(angle), cos(angle), 0},
                    {0, 0, 1 },
                }
            );
    }

    return vec;
}
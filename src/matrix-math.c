#include "../include/matrix-math.h"

Vec3 MatrixVec3Multiplication(Vec3 vec, int matrixSize, double  matrix[matrixSize][matrixSize]) {
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

Vec4 MatrixVec4Multiplication(Vec4 vec, int matrixSize, double matrix[matrixSize][matrixSize]) {
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
#ifndef MATRIX_MATH_H
#define MATRIX_MATH_H
#include "custom-types.h"

Vec3 MatrixVec3Multiplication(Vec3 vec, int matrixSize, float matrix[matrixSize][matrixSize]);

Vec4 MatrixVec4Multiplication(Vec4 vec, int matrixSize, float matrix[matrixSize][matrixSize]);

Vec3 AddVec3(Vec3 vec1, Vec3 vec2);

Vec3 RotateVec3AroundAxis(Vec3 vec, float angle, RotationAxis axis);
#endif
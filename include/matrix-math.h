#ifndef MATRIX_MATH_H
#define MATRIX_MATH_H
#include "custom-types.h"

Vec3 MatrixVec3Multiplication(Vec3 vec, int matrixSize, double matrix[matrixSize][matrixSize]);

Vec4 MatrixVec4Multiplication(Vec4 vec, int matrixSize, double matrix[matrixSize][matrixSize]);
#endif
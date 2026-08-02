#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <math.h>

#define PI 3.1415926

typedef struct {
    double x1;
    double y1;
    double x2;
    double y2;
} BoundingBox;

typedef struct {
    double x;
    double y;
    double z;
    double w;
} ClipCoords, Vec4;

typedef struct {
    double x;
    double y;
    double z;
} WindowCoords, Fragment, Vertex, Vec3, NdCoords;

typedef struct {
    Fragment *data;
    int count;
} FragmentList;

typedef struct {
    double currentDepth;
} Cell;

typedef struct {
    Vertex vertices[3];
} Triangle;

typedef enum {
    X_AXIS,
    Y_AXIS,
    Z_AXIS,
} RotationAxis, MovementAxis;

typedef struct {
    Vec3 pos;
    double pitch;
    double yaw;
} Camera;

#endif
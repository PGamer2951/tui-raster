#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <math.h>

typedef struct {
    float x1;
    float y1;
    float x2;
    float y2;
} BoundingBox;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} ClipCoords, Vec4;

typedef struct {
    float x;
    float y;
    float z;
} WindowCoords, Fragment, Vec3, NdCoords;

typedef struct {
    Vec3 pos;
    Vec3 nor;
    Vec3 uv;
} Vertex;

typedef struct {
    float currentDepth;
} Cell;

typedef struct {
    Vertex vertices[3];
} Triangle;

typedef struct {
    Triangle *mesh;
    int triangleCount;
    Vec3 position;
    Vec3 rotation;
} Object;

typedef enum {
    X_AXIS,
    Y_AXIS,
    Z_AXIS,
} RotationAxis, MovementAxis;

typedef struct {
    char *data;
    int width;
    int height;
} TextBuffer;

typedef struct {
    Vec3 pos;
    float pitch;
    float yaw;
    float speed;
    float rotationSpeed;
} Camera;

#endif
#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

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

#endif
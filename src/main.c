#include <math.h>

#define TB_IMPL
#include <termbox2.h>

#include "../include/obj-loader.h"
#include "../include/matrix-math.h"

int width, height;

int wireframeMode = 0; // 0 -> off | 1 -> on

double perspectiveMatrix[4][4];

void InitPerspectiveMatrix() {
    // Initialize the perspective matrix to use in Clip Space Transformation
    double fov = 70.0 * PI / 180.0; // Convert fov from degrees to radians
    double aspect = (double)width / height;

    double fy = 1.0 / tan(fov / 2.0);
    double fx = fy / aspect;

    double zNear = 0.2;
    double zFar = 4.0;

    double clip1 = (zFar + zNear) / (zNear - zFar);
    double clip2 = (2 * zFar * zNear) / (zNear - zFar);

    memset(&perspectiveMatrix, 0,16*sizeof(double));

    perspectiveMatrix[0][0] = fx;
    perspectiveMatrix[1][1] = fy;
    perspectiveMatrix[2][2] = clip1;
    perspectiveMatrix[2][3] = clip2;
    perspectiveMatrix[3][2] = -1.0;
}

Vertex RotateVertexAroundAxis(Vertex vx, double angle, RotationAxis axis) {
    switch (axis) {
        case X_AXIS:
            return MatrixVec3Multiplication(vx, 3, 
                (double[3][3]){
                    {1, 0, 0},
                    {0, cos(angle), - sin(angle)},
                    {0, sin(angle), cos(angle)},
                }
            );
        case Y_AXIS:
            return MatrixVec3Multiplication(vx, 3, 
                (double[3][3]){
                    {cos(angle), 0, sin(angle)},
                    {0, 1, 0},
                    {- sin(angle), 0, cos(angle)},
                }
            );
        case Z_AXIS:
            return MatrixVec3Multiplication(vx, 3, 
                (double[3][3]){
                    {cos(angle), - sin(angle), 0},
                    {sin(angle), cos(angle), 0},
                    {0, 0, 1 },
                }
            );
        default:
            return vx;
    }
}

Vertex TranslateVertexAlongAxis(Vertex vx, double n, MovementAxis axis) {
    switch (axis) {
        case X_AXIS:
            return (Vertex){ vx.x + n, vx.y, vx.z };
        case Y_AXIS:
            return (Vertex){ vx.x, vx.y + n, vx.z };
        case Z_AXIS:
            return (Vertex){ vx.x, vx.y, vx.z + n };
        default:
            return vx;
    }
}

ClipCoords ClipSpaceTransform(Vertex vx) {
    // Multiply vertex coordinates by perspective matrix -> return clip coordinates
    return MatrixVec4Multiplication((Vec4){ vx.x, vx.y, vx.z, 1.0}, 4, perspectiveMatrix);
}

NdCoords NormalizeDeviceCoordinates(ClipCoords cc) {
    // Divide cc's x, y and z components by w -> return normalized device coordinates
    return (NdCoords){
        cc.x / cc.w,
        cc.y / cc.w,
        cc.z / cc.w,
    };
}

WindowCoords WindowTransformation(NdCoords nc) {
    // Convert normalized device coordinates to window coordinates -> return window coordinates
    return (WindowCoords) {
        (nc.x + 1) / 2 * width,
        (nc.y + 1) / 2 * height,
        (nc.z + 1) / 2,
    };
}

int max(int a, int b, int c) {
    int _max = a > b ? a : b;
    return _max > c ? _max : c;
}

int min(int a, int b, int c) {
    int _min = a < b ? a : b;
    return _min < c ? _min : c;
}

int clamp(int n, int max, int min) {
    if (n < max && n > min) {
        return n;
    }
    else if (n > max) {
        return max;
    }

    return min;
}

int MaxFragmentsInTriangle(WindowCoords wc[3], BoundingBox *bb) {
    // Calculates the max amount of fragments a given triangle could have based on it's vertices
    // Also updates the triangle's bounding box for faster scan conversion
    int minX, maxX, minY, maxY = 0;

    minX = min(wc[0].x, wc[1].x, wc[2].x);
    maxX = max(wc[0].x, wc[1].x, wc[2].x);
    minY = min(wc[0].y, wc[1].y, wc[2].y);
    maxY = max(wc[0].y, wc[1].y, wc[2].y);

    //rintf("min X: %d\nmax X: %d\nmin Y: %d\nmaxY: %d\n", minX, maxX, minY, maxY);

    bb->x1 = clamp(minX, width, 0);
    bb->x2 = clamp(maxX, width, 0);;
    bb->y1 = clamp(minY, height, 0);;
    bb->y2 = clamp(maxY, height, 0);;

    return (maxX - minX) * (maxY - minY);
}

int ScanConversion(WindowCoords *wc, Fragment *frags, BoundingBox *bb, Cell *cells) {
    // scan convert a triangle (3 window coordinates -> 1 triangle)
    WindowCoords wc1 = wc[0];
    WindowCoords wc2 = wc[1];
    WindowCoords wc3 = wc[2];

    int count = 0;
    for (int x = bb->x1; x < bb->x2; x++) {
        for (int y = bb->y1; y < bb->y2; y++) {
            double det = ((wc2.y-wc3.y)*(wc1.x-wc3.x)+(wc3.x-wc2.x)*(wc1.y-wc3.y));

            double lambda1 = (
                ((wc2.y-wc3.y)*(x-wc3.x)+(wc3.x-wc2.x)*(y-wc3.y)) / det
            );
            double lambda2 = (
                ((wc3.y-wc1.y)*(x-wc3.x)+(wc1.x-wc3.x)*(y-wc3.y)) / det
            );
            double lambda3 = (
                1 - lambda1 - lambda2
            );

            if (lambda1 >= 0 && lambda2 >= 0 && lambda3 >= 0) {
                // Check if rendering in wireframe mode
                if (wireframeMode == 0) {
                    // Render normally
                    double depth = wc1.z * lambda1 + wc2.z * lambda2 + wc3.z * lambda3;
                    if (depth < cells[x + (y * width)].currentDepth) {
                        frags[count] = (Fragment){
                            x,
                            y,
                            depth,
                        };
                        count++;
                        cells[x + (y * width)].currentDepth = depth;
                    }
                }
                else if (lambda1 <= 0.05 || lambda2 <= 0.05 || lambda3 <= 0.05) {
                    // Render in wireframe mode
                    double depth = wc1.z * lambda1 + wc2.z * lambda2 + wc3.z * lambda3;
                    if (depth < cells[x + (y * width)].currentDepth) {
                        frags[count] = (Fragment){
                            x,
                            y,
                            depth,
                        };
                        count++;
                        cells[x + (y * width)].currentDepth = depth;
                    }
                }
            }

        }
    }

    return count;
}

void FragmentWriting(Fragment *frags, int count) {
    // write each fragment to the screen with termbox2
    for (int f = 0; f < count; f++) {
        uint32_t unicode;
        char ch = '#';
        uintattr_t color = 0x0000;
        tb_utf8_char_to_unicode(&unicode, &ch);
        tb_set_cell(frags[f].x, frags[f].y, unicode, color, 0);
    }
}

void ClearDepthBuffer(Cell *cells) {
    // Initialize every cell with a depth of -4.0
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            cells[x + (y * width)].currentDepth = 1.1;
        }
    }
}

int main(void) {
    Triangle *mesh = malloc(sizeof(Triangle) * 100);
    int triangleCount = LoadFromFile("data/cube.obj", mesh);

    if (tb_init() != TB_OK) { return 1; } // initialize termbox2
    tb_set_output_mode(2); // allow termbox2 to use more colors

    struct tb_event ev;
    int running = 1;

    width = tb_width();
    height = tb_height();

    InitPerspectiveMatrix();

    WindowCoords finalWC[3];
    BoundingBox box;

    // main loop

    double angle = 1.0;
    while (running == 1) {
        tb_clear();
        // --- rasterisation stuff ---
        width = tb_width();
        height = tb_height();

        Cell *screenCells = malloc(width * height * sizeof(Cell));
        ClearDepthBuffer(screenCells);

        for (int i = 0; i < triangleCount; i++) {
            ClipCoords c0 = ClipSpaceTransform(TranslateVertexAlongAxis(RotateVertexAroundAxis(mesh[i].vertices[0], angle * PI / 180.0, Y_AXIS), -1.5, Z_AXIS));
            ClipCoords c1 = ClipSpaceTransform(TranslateVertexAlongAxis(RotateVertexAroundAxis(mesh[i].vertices[1], angle * PI / 180.0, Y_AXIS), -1.5, Z_AXIS));
            ClipCoords c2 = ClipSpaceTransform(TranslateVertexAlongAxis(RotateVertexAroundAxis(mesh[i].vertices[2], angle * PI / 180.0, Y_AXIS), -1.5, Z_AXIS));

            if(c0.w <= 0.0 || c1.w <= 0.0 || c2.w <= 0.0) {
                continue;
            }

            finalWC[0] = WindowTransformation(NormalizeDeviceCoordinates(c0));
            finalWC[1] = WindowTransformation(NormalizeDeviceCoordinates(c1));
            finalWC[2] = WindowTransformation(NormalizeDeviceCoordinates(c2));

            Fragment *frags = malloc(MaxFragmentsInTriangle(finalWC, &box) * sizeof(Fragment));

            int count = ScanConversion(finalWC, frags, &box, screenCells);

            FragmentWriting(frags, count);

            free(frags);
        }

        free(screenCells);

        tb_present();

        tb_peek_event(&ev, 10);

        if (ev.key == TB_KEY_CTRL_C) {
            running = 0;
        }

        angle++;
    }

    tb_shutdown();

    free(mesh);

    return 0;
}
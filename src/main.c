#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TB_IMPL
#include <termbox2.h>

#define PI 3.1415926

typedef struct {
    double x;
    double y;
    double z;
} vertex, ndCoords;

typedef struct {
    vertex vertices[3];
} triangle;

typedef struct {
    double x;
    double y;
    double z;
    double w;
} clipCoords;

typedef struct {
    double x, y; // X Y -> window coordinates
    double z;    // z -> depth
} windowCoords, fragment;

typedef struct {
    fragment *data;
    size_t count;
} fragmentList;

double perspectiveMatrix[4][4];

void InitPerspectiveMatrix() {
    // Initialize the perspective matrix to use in Clip Space Transformation
    double fov = 70.0 * PI / 180.0; // Convert fov from degrees to radians
    double aspect = (double)tb_width() / tb_height();

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

clipCoords ClipSpaceTransform(vertex vx) {
    // Multiply vertex coordinates by perspective matrix -> return clip coordinates
    clipCoords ret = {
        vx.x * perspectiveMatrix[0][0],
        vx.y * perspectiveMatrix[1][1],
        vx.z * perspectiveMatrix[2][2] + perspectiveMatrix[2][3],
        vx.z * perspectiveMatrix[3][2],
    };
    return ret;
}

ndCoords NormalizeDeviceCoordinates(clipCoords cc) {
    // Divide cc's x, y and z components by w -> return normalized device coordinates
    ndCoords ret = {
        cc.x / cc.w,
        cc.y / cc.w,
        cc.z / cc.w,
    };
    return ret;
}


windowCoords WindowTransformation(ndCoords nc) {
    // Convert normalized device coordinates to window coordinates -> return window coordinates
    windowCoords ret = {
        (nc.x + 1) / 2 * tb_width(),
        (nc.y + 1) / 2 * tb_height(),
        (nc.z + 1) / 2,
    };
    return ret;
}

int max(int a, int b, int c) {
    int _max = a > b ? a : b;
    return _max > c ? _max : c;
}

int min(int a, int b, int c) {
    int _min = a < b ? a : b;
    return _min < c ? _min : c;
}

int MaxFragmentsInTriangle(windowCoords wc[3]) {
    int minX, maxX, minY, maxY = 0;

    minX = min(wc[0].x, wc[1].x, wc[2].x);
    maxX = max(wc[0].x, wc[1].x, wc[2].x);
    minY = min(wc[0].y, wc[1].y, wc[2].y);
    maxY = max(wc[0].y, wc[1].y, wc[2].y);

    return (maxX - minX) * (maxY - minY);
}

int ScanConversion(windowCoords *wc, fragment *frags) {
    // scan convert a trinangle (3 window coordinates -> 1 triangle) -> return a list of fragments
    int width = tb_width();
    int height = tb_height();

    windowCoords wc1 = wc[0];
    windowCoords wc2 = wc[1];
    windowCoords wc3 = wc[2];

    int count = 0;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            double lambda1 = (
                ((wc2.y-wc3.y)*(x-wc3.x)+(wc3.x-wc2.x)*(y-wc3.y)) / ((wc2.y-wc3.y)*(wc1.x-wc3.x)+(wc3.x-wc2.x)*(wc1.y-wc3.y))
            );
            double lambda2 = (
                ((wc3.y-wc1.y)*(x-wc3.x)+(wc1.x-wc3.x)*(y-wc3.y)) / ((wc2.y-wc3.y)*(wc1.x-wc3.x)+(wc3.x-wc2.x)*(wc1.y-wc3.y))
            );
            double lambda3 = (
                1 - lambda1 - lambda2
            );

            if (lambda1 >= 0 && lambda2 >= 0 && lambda3 >= 0) {
                frags[count] = (fragment){
                    x,
                    y,
                    wc1.z * lambda1 + wc2.z * lambda2 + wc3.z * lambda3,
                };
                count++;
            }
        }
    }

    return count;
}

void FragmentWriting(fragment *frags, int count) {
    // write each fragment to the screen with termbox2
    for (int f = 0; f < count; f++) {
        uint32_t unicode;
        char ch = '#';
        tb_utf8_char_to_unicode(&unicode, &ch);
        tb_set_cell(frags[f].x, frags[f].y, unicode, TB_WHITE, 0);
    }
}

int main(void) {
    if (tb_init() != TB_OK) { return 1; } // initialize termbox2
    tb_set_output_mode(2); // allow termbox2 to use more colors

    struct tb_event ev;
    int running = 1;

    InitPerspectiveMatrix();

    triangle triangles[] = {
        // Camera is staring down the -Z axis
        //   -> z = -0.2 : depth = 0; (Near clip)
        //   -> z = -4.0 : depth = 1; (Far clip)
        {
            {
                {-1.0, -1.0, -2.0},
                {1.0, -1.0, -2.0},
                {0.0, 1.0, -2.0},
            }
        },
    };

    // main loop
    while (running == 1) {
        tb_clear();
        // --- rasterisation stuff ---
        for (int i = 0; i < 1; i++) {
            windowCoords *finalWC = malloc(3 * sizeof(windowCoords));

            for (int v = 0; v < 3; v++) {
                finalWC[v] = WindowTransformation(
                    NormalizeDeviceCoordinates(
                        ClipSpaceTransform(triangles[i].vertices[v])
                    )
                );
            }

            fragment *frags = malloc(MaxFragmentsInTriangle(finalWC) * sizeof(fragment));

            int count = ScanConversion(finalWC, frags);

            FragmentWriting(frags, count);

            free(finalWC);
            free(frags);
        }

        tb_present();

        tb_poll_event(&ev);

        if (ev.key == TB_KEY_CTRL_C) {
            running = 0;
        }
    }

    tb_shutdown();

    return 0;
}
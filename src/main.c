#include <math.h>
#include <stdio.h>
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

fragmentList ScanConversion(windowCoords wc[3]) {
    // scan convert a trinangle (3 window coordinates -> 1 triangle) -> return a pointer to a list of fragments
    // CALL FREE() AFTER USING FRAGMENTS
}

void FragmentWriting(fragment *fg) {
    // write each fragment to the screen with termbox
}

int main(void) {
    if (tb_init() != TB_OK) { return 1; } // initialize termbox2
    tb_set_output_mode(2); // allow termbox2 to use more colors

    struct tb_event ev;
    int running = 1;

    InitPerspectiveMatrix();

    triangle tri = {
        {
            {-1.0, -1.0, -2.0},
            {1.0, -1.0, -2.0},
            {0.0, 1.0, -2.0},
        }
    };

    // main loop
    while (running == 1) {
        tb_clear();
        // --- rasterisation stuff ---
        windowCoords finalWC[3];

        for (int v = 0; v < 3; v++) {
            finalWC[v] = WindowTransformation(
                NormalizeDeviceCoordinates(
                    ClipSpaceTransform(tri.vertices[v])
                )
            );

            //printf("Vertex %d -> X: %f | Y: %f | Z: %f\n", v, finalWC[v].x, finalWC[v].y, finalWC[v].z);
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
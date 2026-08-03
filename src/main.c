#include "../include/obj-loader.h"
#include "../include/matrix-math.h"
#include "../include/term.h"

#define PI 3.1415926

struct termios original;

int width, height;

int wireframeMode = 1;

double perspectiveMatrix[4][4];

// Initialize the perspective matrix to use in Clip Space Transformation.
void UpdatePerspectiveMatrix() {
    double fov = 30.0 * PI / 180.0; // Convert fov from degrees to radians
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

// Rotate a vertex by angle around an axis (x, y or z).
Vertex RotateVertexAroundAxis(Vertex vx, double angle, RotationAxis axis) {
    Vertex out = vx;

    switch (axis) {
        case X_AXIS:
            out.pos = MatrixVec3Multiplication(vx.pos, 3, 
                (double[3][3]){
                    {1, 0, 0},
                    {0, cos(angle), - sin(angle)},
                    {0, sin(angle), cos(angle)},
                }
            );
            break;
        case Y_AXIS:
            out.pos = MatrixVec3Multiplication(vx.pos, 3, 
                (double[3][3]){
                    {cos(angle), 0, sin(angle)},
                    {0, 1, 0},
                    {- sin(angle), 0, cos(angle)},
                }
            );
            break;
        case Z_AXIS:
            out.pos = MatrixVec3Multiplication(vx.pos, 3, 
                (double[3][3]){
                    {cos(angle), - sin(angle), 0},
                    {sin(angle), cos(angle), 0},
                    {0, 0, 1 },
                }
            );
            break;
    }

    return out;
}

// Rotate a vertex around all axis at once.
Vertex RotateVertex(Vertex vx, double x, double y, double z) {
    return RotateVertexAroundAxis(RotateVertexAroundAxis(RotateVertexAroundAxis(vx, z, Z_AXIS), y, Y_AXIS), x, X_AXIS);
}

// Move a vertex by n in a given axis (x, y or z).
Vertex TranslateVertexAlongAxis(Vertex vx, double n, MovementAxis axis) {
    Vertex out = vx;

    switch (axis) {
        case X_AXIS:
            out.pos.x += n;
            break;
            //return (Vertex){ vx.pos.x + n, vx.pos.y, vx.pos.z };
        case Y_AXIS:
            out.pos.y += n;
            break;
            //return (Vertex){ vx.pos.x, vx.pos.y + n, vx.pos.z };
        case Z_AXIS:
            out.pos.z += n;
            break;
            //return (Vertex){ vx.pos.x, vx.pos.y, vx.pos.z + n };
    }

    return out;
}

// Move a vertex in all 3 axis at once.
Vertex TranslateVertex(Vertex vx, double x, double y, double z) {
    return TranslateVertexAlongAxis(TranslateVertexAlongAxis(TranslateVertexAlongAxis(vx, x, X_AXIS), z, Z_AXIS), y, Y_AXIS);
}

// Move and rotate a vertex based on it's position and rotation.
Vertex LocalTransform(Vertex vx, Object obj) {
    Vertex rotated = RotateVertex(vx, obj.rotation.x, obj.rotation.y, obj.rotation.z);
    return TranslateVertex(rotated, obj.position.x, obj.position.y, obj.position.z);
}

// Move and rotate a vertex based on the camera's position, yaw and pitch.
Vertex ViewTransfrom(Vertex vx, Camera cam) {
    Vertex moved = TranslateVertex(vx, -cam.pos.x, -cam.pos.y, -cam.pos.z);
    return RotateVertex(moved, -cam.pitch, -cam.yaw, 0.0);
}

// Multiply vertex coordinates by perspective matrix -> return clip coordinates.
ClipCoords ClipSpaceTransform(Vertex vx) {
    return MatrixVec4Multiplication((Vec4){ vx.pos.x, vx.pos.y, vx.pos.z, 1.0}, 4, perspectiveMatrix);
}

// Divide cc's x, y and z components by w -> return normalized device coordinates.
NdCoords NormalizeDeviceCoordinates(ClipCoords cc) {
    return (NdCoords){
        cc.x / cc.w,
        cc.y / cc.w,
        cc.z / cc.w,
    };
}

// Convert normalized device coordinates to window coordinates -> return window coordinates.
WindowCoords WindowTransformation(NdCoords nc) {
    return (WindowCoords) {
        (nc.x + 1) / 2 * width,
        (1.0 - (nc.y + 1) / 2) * height,
        (nc.z + 1) / 2,
    };
}

// Returns max integer out of the three integers given.
int max(int a, int b, int c) {
    int _max = a > b ? a : b;
    return _max > c ? _max : c;
}

// Returns min integer out of the three integers given.
int min(int a, int b, int c) {
    int _min = a < b ? a : b;
    return _min < c ? _min : c;
}

// Clamps an integer value n between a max and min value.
int clamp(int n, int max, int min) {
    if (n < max && n > min) {
        return n;
    }
    else if (n > max) {
        return max;
    }

    return min;
}

// Calculates the triangle's bounding box for faster scan conversion.
void CalculateBoundingBox(WindowCoords wc[3], BoundingBox *bb) {
    int minX, maxX, minY, maxY;

    minX = min(wc[0].x, wc[1].x, wc[2].x);
    maxX = max(wc[0].x, wc[1].x, wc[2].x);
    minY = min(wc[0].y, wc[1].y, wc[2].y);
    maxY = max(wc[0].y, wc[1].y, wc[2].y);

    bb->x1 = clamp(minX, width, 0);
    bb->x2 = clamp(maxX, width, 0);;
    bb->y1 = clamp(minY, height, 0);;
    bb->y2 = clamp(maxY, height, 0);;
}

// Scan convert a triangle (3 window coordinates -> 1 triangle).
int ScanConversion(WindowCoords *wc, Fragment *frags, BoundingBox *bb, Cell *cells) {
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

// Map a depth value to the corresponding char to render on the terminal.
char DepthToChar(double depth) {
    // @ % # * + = - : .
    char chars[4] = "@%#?";

    double targetDepth = 0.25;

    for (int i = 0; i < 4; i++) {
        if (depth < targetDepth) return chars[i];
        targetDepth += 0.25;
    }

    return '=';
}

// Write each fragment to the screen with the custom term.h terminal renderer.
void FragmentWriting(Fragment *frags, TextBuffer *buf, int count) {
    for (int f = 0; f < count; f++) {
        AddToBuffer(buf, frags[f].x, frags[f].y, DepthToChar(frags[f].z));
    }
}

// Initialize every cell with a depth of -4.0
void ClearDepthBuffer(Cell *cells) {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            cells[x + (y * width)].currentDepth = 1.1;
        }
    }
}

int main(void) {
    //Triangle *test = malloc(sizeof(Triangle) * 1000);
    //int val = LoadFromFile("data/suzanne.obj", test);
    //return 0;

    // Loading scene
    Object car = {
        malloc(sizeof(Triangle) * 1000),
        0,
        {2.0, 0.0, -4.0},
        {0.0, 0.0, 0.0},
    };
    car.triangleCount = LoadFromFile("data/Car.obj", car.mesh);

    Object monkey = {
        malloc(sizeof(Triangle) * 1000),
        0,
        {-1.0, 0.5, 2.0},
        {0.0, 0.0, 0.0}
    };
    monkey.triangleCount = LoadFromFile("data/suzanne.obj", monkey.mesh);

    Object scene[2] = {monkey, car};

    // Terminal setup
    tcgetattr(STDIN_FILENO, &original);
    InitTerm(&original);

    int running = 1;

    width = TermWidth();
    height = TermHeight();

    // Initialize perspective matrix
    UpdatePerspectiveMatrix();

    // Text buffer for terminal and depth buffer setup
    TextBuffer buffer = {
        malloc(width * height),
        width,
        height,
    };

    Cell *screenCells = malloc(width * height * sizeof(Cell));

    WindowCoords finalWC[3];
    BoundingBox box;
    Fragment *frags = malloc(width * height * sizeof(Fragment));

    // Setup camera
    Camera cam = { 
        { 0.0, 0.0, 0.0 },
        0.0,
        0.0,
        0.1,
    };

    // Main loop
    while (running == 1) {
        // Update width and height values if necessary and recalculate everything that depends on those values
        if (width != TermWidth() || height != TermHeight()) {
            width = TermWidth();
            height = TermHeight();

            UpdatePerspectiveMatrix();

            screenCells = realloc(screenCells, width * height * sizeof(Cell));

            buffer.data = realloc(buffer.data, width * height * sizeof(char));
            buffer.width = width;
            buffer.height = height;

            frags = realloc(frags, width * height * sizeof(Fragment));
        }

        // Clear text and depth buffers
        ClearBuffer(&buffer);
        ClearDepthBuffer(screenCells);

        // Main rendering
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < scene[0].triangleCount; j++) {
                ClipCoords c0 = ClipSpaceTransform(ViewTransfrom(LocalTransform(scene[i].mesh[j].vertices[0], scene[i]), cam));
                ClipCoords c1 = ClipSpaceTransform(ViewTransfrom(LocalTransform(scene[i].mesh[j].vertices[1], scene[i]), cam));
                ClipCoords c2 = ClipSpaceTransform(ViewTransfrom(LocalTransform(scene[i].mesh[j].vertices[2], scene[i]), cam));

                if (c0.w <= 0 || c1.w <= 0 || c2.w <= 0) {
                    continue;
                }

                finalWC[0] = WindowTransformation(NormalizeDeviceCoordinates(c0));
                finalWC[1] = WindowTransformation(NormalizeDeviceCoordinates(c1));
                finalWC[2] = WindowTransformation(NormalizeDeviceCoordinates(c2));

                CalculateBoundingBox(finalWC, &box);
                int count = ScanConversion(finalWC, frags, &box, screenCells);
                FragmentWriting(frags, &buffer, count);
            }
        }

        // Present buffer to terminal
        PresentBuffer(&buffer);

        // Input handling and camera movement + rotation
        char c[3];
        if(PeekInput(c) == OK) {
            Vec3 forward = RotateVec3AroundAxis((Vec3){0.0, 0.0, -cam.speed}, cam.yaw, Y_AXIS);
            Vec3 backward = RotateVec3AroundAxis((Vec3){0.0, 0.0, cam.speed}, cam.yaw, Y_AXIS);
            Vec3 right = RotateVec3AroundAxis((Vec3){cam.speed, 0.0, 0.0}, cam.yaw, Y_AXIS);
            Vec3 left = RotateVec3AroundAxis((Vec3){-cam.speed, 0.0, 0.0}, cam.yaw, Y_AXIS);

            switch (c[0]) {
                case 'q':
                    running = 0;
                    break;
                case 'w':
                    cam.pos = AddVec3(cam.pos, forward);
                    break;
                case 'a':
                    cam.pos = AddVec3(cam.pos, left);
                    break;
                case 's':
                    cam.pos = AddVec3(cam.pos, backward);
                    break;
                case 'd':
                    cam.pos = AddVec3(cam.pos, right);
                    break;
                case 'r':
                    cam.pos.y += cam.speed;
                    break;
                case 'f':
                    cam.pos.y -= cam.speed;
                    break;
                case 'i':
                    cam.pitch += cam.speed;
                    break;
                case 'k':
                    cam.pitch -= cam.speed;
                    break;
                case 'j':
                    cam.yaw += cam.speed;
                    break;
                case 'l':
                    cam.yaw -= cam.speed;
                    break;
            }
        }
    }

    // Memory cleanup and return terminal to original form
    ShutdownTerm(&original);

    free(buffer.data);
    free(screenCells);
    free(frags);
    free(monkey.mesh);

    return 0;
}
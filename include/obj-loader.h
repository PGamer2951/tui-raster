typedef struct {
    double x;
    double y;
    double z;
} vertex;

typedef struct {
    vertex vertices[3];
} triangle;

int LoadFromFile(char *path, triangle *mesh);

vertex GetVertexFromText(char *text);
triangle GetTriangleFromText(char *text, vertex *vertices);
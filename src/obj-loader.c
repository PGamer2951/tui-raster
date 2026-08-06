#include "../include/obj-loader.h"

int LoadFromFile(char *path, Triangle *mesh) {
    Vec3 positions[1000];
    Vec3 normals[1000];
    Vec3 textures[1000];

    FILE *ptr = fopen(path, "r");

    if (ptr == NULL) {
        return 0;
    }

    char data[100];

    int vertexCount = 0;
    int normalCount = 0;
    int textureCount = 0;

    int triangleCount = 0;
    
    while (fgets(data, sizeof(data), ptr)) {
        if (data[0] == 'v') {
            if (data[1] == ' ') {
                sscanf(data, "v %f %f %f", &positions[vertexCount].x, &positions[vertexCount].y, &positions[vertexCount].z);
                vertexCount++;
            }
            else if (data[1] == 'n') {
                sscanf(data, "vn %f %f %f", &normals[normalCount].x, &normals[normalCount].y, &normals[normalCount].z);
                normalCount++;
            }
            else if (data[1] == 't') {
                sscanf(data, "vt %f %f %f", &textures[textureCount].x, &textures[textureCount].y, &textures[textureCount].z);
                textureCount++;
            }
        }
        else if (data[0] == 'f') {
            char copy[100];
            strcpy(copy, data);

            char *split = strtok(copy, " ");
            int n = 0;

            while (split != NULL) {
                n++;
                split = strtok(NULL, " ");
            }

            if (n == 4) {
                int p1, n1, t1,
                    p2, n2, t2,
                    p3, n3, t3;

                sscanf(data, "f %d/%d/%d %d/%d/%d %d/%d/%d", &p1, &t1, &n1, &p2, &t2, &n2, &p3, &t3, &n3);

                mesh[triangleCount].vertices[0].pos = positions[p1 - 1];
                mesh[triangleCount].vertices[0].nor = normals[n1 - 1];
                mesh[triangleCount].vertices[0].uv = textures[t1 - 1];

                mesh[triangleCount].vertices[1].pos = positions[p2 - 1];
                mesh[triangleCount].vertices[1].nor = normals[n2 - 1];
                mesh[triangleCount].vertices[1].uv = textures[t2 - 1];

                mesh[triangleCount].vertices[2].pos = positions[p3 - 1];
                mesh[triangleCount].vertices[2].nor = normals[n3 - 1];
                mesh[triangleCount].vertices[2].uv = textures[t3 - 1];

                triangleCount++;
            }
            else if (n == 5) {
                int p1, n1, t1,
                    p2, n2, t2,
                    p3, n3, t3,
                    p4, n4, t4;

                sscanf(data, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &p1, &t1, &n1, &p2, &t2, &n2, &p3, &t3, &n3, &p4, &t4, &n4);

                mesh[triangleCount].vertices[0].pos = positions[p1 - 1];
                mesh[triangleCount].vertices[0].nor = normals[n1 - 1];
                mesh[triangleCount].vertices[0].uv = textures[t1 - 1];

                mesh[triangleCount].vertices[1].pos = positions[p2 - 1];
                mesh[triangleCount].vertices[1].nor = normals[n2 - 1];
                mesh[triangleCount].vertices[1].uv = textures[t2 - 1];

                mesh[triangleCount].vertices[2].pos = positions[p3 - 1];
                mesh[triangleCount].vertices[2].nor = normals[n3 - 1];
                mesh[triangleCount].vertices[2].uv = textures[t3 - 1];

                triangleCount++;

                mesh[triangleCount].vertices[0].pos = positions[p1 - 1];
                mesh[triangleCount].vertices[0].nor = normals[n1 - 1];
                mesh[triangleCount].vertices[0].uv = textures[t1 - 1];

                mesh[triangleCount].vertices[1].pos = positions[p3 - 1];
                mesh[triangleCount].vertices[1].nor = normals[n3 - 1];
                mesh[triangleCount].vertices[1].uv = textures[t3 - 1];

                mesh[triangleCount].vertices[2].pos = positions[p4 - 1];
                mesh[triangleCount].vertices[2].nor = normals[n4 - 1];
                mesh[triangleCount].vertices[2].uv = textures[t4 - 1];

                triangleCount++;
            }
        }
    }

    fclose(ptr);
    
    return triangleCount;
}
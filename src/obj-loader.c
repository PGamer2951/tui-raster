#include "../include/obj-loader.h"

int LoadFromFile(char *path, Triangle *mesh) {
    // TO-DO: WORK ON OBJ LOADER
    Vec3 positions[1000];
    Vec3 normals[1000];
    Vec3 textures[1000];

    FILE *ptr = fopen(path, "r");

    if (ptr == NULL) {
        printf("Failed to open file at: %s\n", path); 
        return 0;
    }

    char data[100];

    int vertexCount = 0;
    int normalCount = 0;
    int textureCount = 0;

    int triangleCount = 0;
    
    while (fgets(data, 49, ptr)) {
        if (data[0] == 'v') {
            if (data[1] == ' ') {
                sscanf(data, "v %lf %lf %lf", &positions[vertexCount].x, &positions[vertexCount].y, &positions[vertexCount].z);
                vertexCount++;
            }
            else if (data[1] == 'n') {
                sscanf(data, "vn %lf %lf %lf", &normals[normalCount].x, &normals[normalCount].y, &normals[normalCount].z);
                normalCount++;
            }
            else if (data[1] == 't') {
                sscanf(data, "vt %lf %lf %lf", &textures[textureCount].x, &textures[textureCount].y, &textures[textureCount].z);
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

                mesh[triangleCount].vertices[0].pos = positions[p2 - 1];
                mesh[triangleCount].vertices[0].nor = normals[n2 - 1];
                mesh[triangleCount].vertices[0].uv = textures[t2 - 1];

                mesh[triangleCount].vertices[1].pos = positions[p3 - 1];
                mesh[triangleCount].vertices[1].nor = normals[n3 - 1];
                mesh[triangleCount].vertices[1].uv = textures[t3 - 1];

                mesh[triangleCount].vertices[2].pos = positions[p4 - 1];
                mesh[triangleCount].vertices[2].nor = normals[n4 - 1];
                mesh[triangleCount].vertices[2].uv = textures[t4 - 1];

                triangleCount++;
            }
            // TO-DO: PROBABLY GET RID OF GetTriangleFromText() FUNCTION AND WRITE EVERYTHING IN THIS ONE TO WRITE DIRECTLY INTO mesh

            //mesh[triangleCount] = GetTriangleFromText(data, positions);
            //triangleCount++;
        }
    }

    fclose(ptr);

    /*
    printf(" Vertex: %d\n", vertexCount);
    printf(" Normal: %d\n", normalCount);
    printf(" Texture: %d\n", textureCount);
    printf("------------------------------\n");
    printf(" Vertex 1 Y: %f\n", positions[0].y);
    printf(" Normal 2 X: %f\n", normals[1].x);
    printf(" Texture 3 Z: %f\n", textures[2].y);
    */

    printf("Loaded %d triangles\n", triangleCount);
    
    return triangleCount;
}

/*
Vertex GetVertexFromText(char *text) {
    char *split = strtok(text, " ");
    Vertex ret;

    int n = 0;

    while (split != NULL && n < 4) {
        switch (n) {
            case 0:
                break;
            case 1:
                ret.x = atof(split);
                break;
            case 2:
                ret.y = atof(split);
                break;
            case 3:
                ret.z = atof(split);
                break;
        }

        n++;
        split = strtok(NULL, " ");
    }

    return ret;
}

Triangle GetTriangleFromText(char *text, Vertex *vertices) {
    // TO-DO: FIND WAY TO DETECT IF FACE IS DECLARED AS TRIANGLE OR QUAD

    char *split = strtok(text, " ");
    Triangle ret;

    int p1, n1, t1,
        p2, n2, t2,
        p3, n3, t3;
    
    sscanf(text, "f %d/%d/%d %d/%d/%d %d/%d/%d", &p1, &n1, &t1, &p2, &n2, &t2, &p3, &n3, &t3);

    // vertex2 v1 = { positions[p1], normals[n1], textures[t1]}



    int i = 0;

    while (split != NULL && i < 4) {
        if (i != 0) {
            char *split2 = strtok(split, "/");
            int j = 0;

            while (split2 != NULL && j < 3) {
                // if (j is 0) -> ret.
            }
            ret.vertices[i - 1] = vertices[atoi(split) - 1];            
        }
        
        i++;
        split = strtok(NULL, " ");
    }

    return ret;
}
*/
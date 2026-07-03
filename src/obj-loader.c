#include "../include/obj-loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int LoadFromFile(char *path, triangle *mesh) {
    printf("Loading file ...");
    vertex vertices[300];

    FILE *ptr = fopen(path, "r");

    if (ptr == NULL) {
        printf("Failed to open file at: %s\n", path); 
        return 0;
    }

    char data[50];

    int vertexCount = 0;
    int triangleCount = 0;
    
    while (fgets(data, 49, ptr)) {
        switch (data[0]) {
            case 'v':
                vertices[vertexCount] = GetVertexFromText(data);
                vertexCount++;
                break;
            case 'f':
                mesh[triangleCount] = GetTriangleFromText(data, vertices);
                triangleCount++;
                break;
        }
    }

    return triangleCount;
}

vertex GetVertexFromText(char *text) {
    char *split = strtok(text, " ");
    vertex ret;

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

    free(split);

    return ret;
}

triangle GetTriangleFromText(char *text, vertex *vertices) {
    char *split = strtok(text, " ");
    triangle ret;

    int n = 0;

    while (split != NULL && n < 4) {
        if (n != 0) {
            ret.vertices[n - 1] = vertices[atoi(split) - 1];            
        }
        
        n++;
        split = strtok(NULL, " ");
    }

    free(split);

    return ret;
}
#include "../include/scene-loader.h"
#include "../include/obj-loader.h"

int LoadSceneFromFile(char *path, Object *scene) {
    FILE *ptr = fopen(path, "r");

    if (ptr == NULL) {
        printf("Failed to load file");
        return 0;
    }

    char data[100];
    int n = 0;

    while (fgets(data, sizeof(data), ptr)) {
        char copy[100];
        strcpy(copy, data);

        char *split = strtok(copy, " ");

        if (strcmp(split, "LOAD") == 0) {
            char fileToLoad[100];

            sscanf(data, "LOAD %s", fileToLoad);

            scene[n].mesh = malloc(1000 * sizeof(Triangle));
            int triCount = LoadFromFile(fileToLoad, scene[n].mesh);
            scene[n].triangleCount = triCount;

            n++;
        }
        else if (strcmp(split, "MOVE") == 0) {
            int idx;
            int x, y, z;

            sscanf(data, "MOVE %d %d/%d/%d", &idx, &x, &y, &z);

            scene[idx].position = (Vec3){x, y, z};
        }
        else if (strcmp(split, "ROTATE") == 0) {
            int idx;
            int x, y, z;

            sscanf(data, "ROTATE %d %d/%d/%d", &idx, &x, &y, &z);

            scene[idx].rotation = (Vec3){x, y, z};
        }
    }

    fclose(ptr);

    return n;
}
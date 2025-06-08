#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Использование: %s <rows> <cols>\n", argv[0]);
        return 1;
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    int **arr1 = (int **)malloc(rows * sizeof(int *));
    int **arr2 = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        arr1[i] = (int *)malloc(cols * sizeof(int));
        arr2[i] = (int *)malloc(cols * sizeof(int));
    }

    srand(time(NULL));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            arr1[i][j] = rand() % 1000 + 1;
            arr2[i][j] = rand() % 1000 + 1;
        }
    }

    clock_t start = clock();
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int sum = arr1[i][j] + arr2[i][j];
            int diff = arr1[i][j] - arr2[i][j];
            int prod = arr1[i][j] * arr2[i][j];
            float div = (float)arr1[i][j] / arr2[i][j];
        }
    }
    clock_t end = clock();

    printf("Время: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    for (int i = 0; i < rows; i++) {
        free(arr1[i]);
        free(arr2[i]);
    }
    free(arr1);
    free(arr2);
    return 0;
}

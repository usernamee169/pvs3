#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Использование: %s <array_size>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);

    int *arr1 = (int *)malloc(size * sizeof(int));
    int *arr2 = (int *)malloc(size * sizeof(int));
    srand(time(NULL));

    for (int i = 0; i < size; i++) {
        arr1[i] = rand() % 1000 + 1;
        arr2[i] = rand() % 1000 + 1;
    }

    clock_t start = clock();
    for (int i = 0; i < size; i++) {
        int sum = arr1[i] + arr2[i];
        int diff = arr1[i] - arr2[i];
        int prod = arr1[i] * arr2[i];
        float div = (float)arr1[i] / arr2[i];
    }
    clock_t end = clock();

    printf("Время: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    free(arr1);
    free(arr2);
    return 0;
}

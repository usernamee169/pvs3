#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void bubble_sort(int *arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <array_size>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    if (size <= 0) {
        printf("Array size must be positive\n");
        return 1;
    }

    int *arr = (int *)malloc(size * sizeof(int));
    srand(time(NULL));

    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 1000;
    }

    clock_t start = clock();
    bubble_sort(arr, size);
    clock_t end = clock();

    printf("First 10 elements: ");
    for (int i = 0; i < 10 && i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\nTime taken: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    free(arr);
    return 0;
}

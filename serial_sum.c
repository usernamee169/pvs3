#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Используется: %s <array_size>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    if (size <= 0) {
        printf("Размер массива положительное значение!\n");
        return 1;
    }

    int *arr = (int *)malloc(size * sizeof(int));
    srand(time(NULL));

    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 1000;
    }

    clock_t start = clock();
    long long sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    clock_t end = clock();

    printf("Сумма: %lld\n", sum);
    printf("Время: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    free(arr);
    return 0;
}

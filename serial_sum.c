#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <размер_массива>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    if (size <= 0) {
        fprintf(stderr, "Размер массива должен быть положительным целым числом.\n");
        return 1;
    }

    int *arr = (int *)malloc(size * sizeof(int));
    if (arr == NULL) {
        perror("Ошибка выделения памяти");
        return 1;
    }

    srand(time(NULL));
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 100; 
    }


    long long sum = 0;
    clock_t start = clock();
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    clock_t end = clock();

    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Последовательная сумма: %lld\n", sum);
    printf("Время выполнения последовательной версии: %f секунд\n", time_spent);

    free(arr);
    return 0;
}

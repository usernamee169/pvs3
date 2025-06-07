#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_array(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void bubble_sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Обмен элементов
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <размер_массива>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]); // Получаем размер массива из аргументов командной строки

    if (n <= 0) {
        fprintf(stderr, "Размер массива должен быть положительным целым числом.\n");
        return 1;
    }

    int *arr = (int *)malloc(n * sizeof(int)); 

    if (arr == NULL) {
        fprintf(stderr, "Ошибка выделения памяти.\n");
        return 1;
    }

    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 1000; 
    }


    clock_t start_time = clock();
    bubble_sort(arr, n);
    clock_t end_time = clock();

    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC; 


    printf("Время сортировки: %f секунд\n", time_taken);

    free(arr); 
    return 0;
}

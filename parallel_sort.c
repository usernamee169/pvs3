#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

void bubble_sort(int* array, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (argc != 2) {
        if (rank == 0) {
            printf("Usage: mpirun -np <processes> %s <array_size>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    int array_size = atoi(argv[1]);
    int* array = NULL;
    double start_time, end_time;
    
    if (rank == 0) {
        array = (int*)malloc(array_size * sizeof(int));
        srand(time(NULL));
        for (int i = 0; i < array_size; i++) {
            array[i] = rand() % 1000;
        }
        start_time = MPI_Wtime();
    }
    
    // Распределяем данные по процессам
    int local_size = array_size / size;
    int* local_array = (int*)malloc(local_size * sizeof(int));
    
    MPI_Scatter(array, local_size, MPI_INT, local_array, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Локальная сортировка
    bubble_sort(local_array, local_size);
    
    // Сбор отсортированных частей
    MPI_Gather(local_array, local_size, MPI_INT, array, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        // Сортировка оставшихся элементов
        int remaining = array_size % size;
        if (remaining > 0) {
            bubble_sort(array + array_size - remaining, remaining);
        }
        
        // Финальная сортировка слиянием
        bubble_sort(array, array_size);
        
        end_time = MPI_Wtime();
        printf("Array size: %d\n", array_size);
        printf("Execution time: %.6f seconds\n", end_time - start_time);
        
        // Проверка сортировки (опционально)
        for (int i = 0; i < array_size - 1; i++) {
            if (array[i] > array[i + 1]) {
                printf("Sorting error at index %d\n", i);
                break;
            }
        }
        
        free(array);
    }
    
    free(local_array);
    MPI_Finalize();
    return 0;
}

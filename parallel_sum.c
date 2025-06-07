#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

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
            array[i] = rand() % 100;
        }
        start_time = MPI_Wtime();
    }
    
    // Распределяем данные по процессам
    int local_size = array_size / size;
    int* local_array = (int*)malloc(local_size * sizeof(int));
    
    MPI_Scatter(array, local_size, MPI_INT, local_array, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Локальное суммирование
    int local_sum = 0;
    for (int i = 0; i < local_size; i++) {
        local_sum += local_array[i];
    }
    
    // Сбор результатов
    int global_sum;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        // Обработка оставшихся элементов
        int remaining = array_size % size;
        for (int i = array_size - remaining; i < array_size; i++) {
            global_sum += array[i];
        }
        
        end_time = MPI_Wtime();
        printf("Array size: %d\n", array_size);
        printf("Total sum: %d\n", global_sum);
        printf("Execution time: %.6f seconds\n", end_time - start_time);
        free(array);
    }
    
    free(local_array);
    MPI_Finalize();
    return 0;
}

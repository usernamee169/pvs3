#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (argc != 3) {
        if (rank == 0) {
            printf("Usage: mpirun -np <processes> %s <rows> <cols>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    int total_elements = rows * cols;
    
    int *array1 = NULL, *array2 = NULL;
    double start_time, end_time;
    
    if (rank == 0) {
        array1 = (int*)malloc(total_elements * sizeof(int));
        array2 = (int*)malloc(total_elements * sizeof(int));
        
        srand(time(NULL));
        for (int i = 0; i < total_elements; i++) {
            array1[i] = rand() % 100 + 1;
            array2[i] = rand() % 100 + 1;
        }
        start_time = MPI_Wtime();
    }
    
    int local_size = total_elements / size;
    int *local_array1 = (int*)malloc(local_size * sizeof(int));
    int *local_array2 = (int*)malloc(local_size * sizeof(int));
    
    MPI_Scatter(array1, local_size, MPI_INT, local_array1, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(array2, local_size, MPI_INT, local_array2, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Выполняем операции
    int *local_add = (int*)malloc(local_size * sizeof(int));
    int *local_sub = (int*)malloc(local_size * sizeof(int));
    int *local_mul = (int*)malloc(local_size * sizeof(int));
    int *local_div = (int*)malloc(local_size * sizeof(int));
    
    for (int i = 0; i < local_size; i++) {
        local_add[i] = local_array1[i] + local_array2[i];
        local_sub[i] = local_array1[i] - local_array2[i];
        local_mul[i] = local_array1[i] * local_array2[i];
        local_div[i] = local_array1[i] / local_array2[i];
    }
    
    // Собираем результаты
    int *global_add = NULL, *global_sub = NULL, *global_mul = NULL, *global_div = NULL;
    
    if (rank == 0) {
        global_add = (int*)malloc(total_elements * sizeof(int));
        global_sub = (int*)malloc(total_elements * sizeof(int));
        global_mul = (int*)malloc(total_elements * sizeof(int));
        global_div = (int*)malloc(total_elements * sizeof(int));
    }
    
    MPI_Gather(local_add, local_size, MPI_INT, global_add, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(local_sub, local_size, MPI_INT, global_sub, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(local_mul, local_size, MPI_INT, global_mul, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(local_div, local_size, MPI_INT, global_div, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        // Обработка оставшихся элементов
        int remaining = total_elements % size;
        for (int i = total_elements - remaining; i < total_elements; i++) {
            global_add[i] = array1[i] + array2[i];
            global_sub[i] = array1[i] - array2[i];
            global_mul[i] = array1[i] * array2[i];
            global_div[i] = array1[i] / array2[i];
        }
        
        end_time = MPI_Wtime();
        printf("Matrix size: %d x %d (%d elements)\n", rows, cols, total_elements);
        printf("Execution time: %.6f seconds\n", end_time - start_time);
        
        // Вывод первых 5 результатов для проверки (опционально)
        printf("First 5 results:\n");
        for (int i = 0; i < 5; i++) {
            int row = i / cols;
            int col = i % cols;
            printf("(%d,%d): %d + %d = %d\n", row, col, array1[i], array2[i], global_add[i]);
            printf("(%d,%d): %d - %d = %d\n", row, col, array1[i], array2[i], global_sub[i]);
            printf("(%d,%d): %d * %d = %d\n", row, col, array1[i], array2[i], global_mul[i]);
            printf("(%d,%d): %d / %d = %d\n", row, col, array1[i], array2[i], global_div[i]);
        }
        
        free(array1);
        free(array2);
        free(global_add);
        free(global_sub);
        free(global_mul);
        free(global_div);
    }
    
    free(local_array1);
    free(local_array2);
    free(local_add);
    free(local_sub);
    free(local_mul);
    free(local_div);
    
    MPI_Finalize();
    return 0;
}

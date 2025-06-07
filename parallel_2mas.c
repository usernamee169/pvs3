#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

void initialize_array(double *array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = (double)rand() / RAND_MAX * 100.0;
    }
}

void perform_operations(double *a, double *b, double *add, double *sub, double *mul, double *div, int size) {
    for (int i = 0; i < size; i++) {
        add[i] = a[i] + b[i];
        sub[i] = a[i] - b[i];
        mul[i] = a[i] * b[i];
        div[i] = b[i] != 0.0 ? a[i] / b[i] : 0.0;
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0) {
            printf("Usage: %s <rows> <cols>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    int total_elements = rows * cols;

    if (rows <= 0 || cols <= 0 || total_elements < 100000) {
        if (rank == 0) {
            printf("Arrays must have at least 100000 elements (rows * cols >= 100000)\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Разделяем работу между процессами
    int elements_per_proc = total_elements / size;
    int remainder = total_elements % size;

    // Определяем количество элементов для текущего процесса
    int local_count = elements_per_proc + (rank < remainder ? 1 : 0);
    int *counts = NULL;
    int *displs = NULL;

    if (rank == 0) {
        counts = (int *)malloc(size * sizeof(int));
        displs = (int *)malloc(size * sizeof(int));

        int offset = 0;
        for (int i = 0; i < size; i++) {
            counts[i] = elements_per_proc + (i < remainder ? 1 : 0);
            displs[i] = offset;
            offset += counts[i];
        }
    }

    // Выделяем память для локальных частей массивов
    double *local_a = (double *)malloc(local_count * sizeof(double));
    double *local_b = (double *)malloc(local_count * sizeof(double));
    double *local_add = (double *)malloc(local_count * sizeof(double));
    double *local_sub = (double *)malloc(local_count * sizeof(double));
    double *local_mul = (double *)malloc(local_count * sizeof(double));
    double *local_div = (double *)malloc(local_count * sizeof(double));

    // Инициализация массивов в процессе 0
    double *a = NULL;
    double *b = NULL;

    if (rank == 0) {
        a = (double *)malloc(total_elements * sizeof(double));
        b = (double *)malloc(total_elements * sizeof(double));
        srand(time(NULL));
        initialize_array(a, total_elements);
        initialize_array(b, total_elements);
    }

    // Разделяем данные между процессами
    MPI_Scatterv(a, counts, displs, MPI_DOUBLE, local_a, local_count, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(b, counts, displs, MPI_DOUBLE, local_b, local_count, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Выполняем операции
    double start_time = MPI_Wtime();
    perform_operations(local_a, local_b, local_add, local_sub, local_mul, local_div, local_count);
    double end_time = MPI_Wtime();

    // Собираем результаты
    double *add = NULL;
    double *sub = NULL;
    double *mul = NULL;
    double *div = NULL;

    if (rank == 0) {
        add = (double *)malloc(total_elements * sizeof(double));
        sub = (double *)malloc(total_elements * sizeof(double));
        mul = (double *)malloc(total_elements * sizeof(double));
        div = (double *)malloc(total_elements * sizeof(double));
    }

    MPI_Gatherv(local_add, local_count, MPI_DOUBLE, add, counts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gatherv(local_sub, local_count, MPI_DOUBLE, sub, counts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gatherv(local_mul, local_count, MPI_DOUBLE, mul, counts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gatherv(local_div, local_count, MPI_DOUBLE, div, counts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Parallel execution time with %d processes: %.4f seconds\n", size, end_time - start_time);

        // Для больших массивов вывод не имеет смысла, поэтому закомментирован
        /*
        printf("First 10 elements of each operation:\n");
        printf("Addition: ");
        for (int i = 0; i < 10; i++) printf("%.2f ", add[i]);
        printf("\nSubtraction: ");
        for (int i = 0; i < 10; i++) printf("%.2f ", sub[i]);
        printf("\nMultiplication: ");
        for (int i = 0; i < 10; i++) printf("%.2f ", mul[i]);
        printf("\nDivision: ");
        for (int i = 0; i < 10; i++) printf("%.2f ", div[i]);
        printf("\n");
        */

        free(a);
        free(b);
        free(add);
        free(sub);
        free(mul);
        free(div);
        free(counts);
        free(displs);
    }

    free(local_a);
    free(local_b);
    free(local_add);
    free(local_sub);
    free(local_mul);
    free(local_div);

    MPI_Finalize();
    return 0;
}

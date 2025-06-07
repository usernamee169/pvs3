#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int rows, cols;
    double *matrix1 = NULL;
    double *matrix2 = NULL;
    double *sum = NULL;
    double *difference = NULL;
    double *product = NULL;
    double *quotient = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        if (argc != 3) {
            fprintf(stderr, "Использование: %s <rows> <cols>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        rows = atoi(argv[1]);
        cols = atoi(argv[2]);

        if (rows <= 0 || cols <= 0 || (long)rows * cols < 100000) {
            fprintf(stderr, "Размеры массивов должны быть положительными и общее количество элементов должно быть не менее 100000.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        matrix1 = (double *)malloc(rows * cols * sizeof(double));
        matrix2 = (double *)malloc(rows * cols * sizeof(double));
        sum = (double *)malloc(rows * cols * sizeof(double));
        difference = (double *)malloc(rows * cols * sizeof(double));
        product = (double *)malloc(rows * cols * sizeof(double));
        quotient = (double *)malloc(rows * cols * sizeof(double));

        if (!matrix1 || !matrix2 || !sum || !difference || !product || !quotient) {
            fprintf(stderr, "Ошибка выделения памяти.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        srand(time(NULL));
        for (int i = 0; i < rows * cols; i++) {
            matrix1[i] = (double)rand() / RAND_MAX;
            matrix2[i] = (double)rand() / RAND_MAX;
        }
    }

    // Рассылаем размеры массивов всем процессам
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Вычисляем размер части массива для каждого процесса
    int chunk_size = (rows * cols) / size;
    int remainder = (rows * cols) % size;

    // Вычисляем начальный и конечный индекс для каждого процесса
    int start_index = rank * chunk_size + (rank < remainder ? rank : remainder);
    int end_index = start_index + chunk_size + (rank < remainder ? 1 : 0);

    // Выделяем память для локальных частей массивов
    double *local_matrix1 = (double *)malloc((end_index - start_index) * sizeof(double));
    double *local_matrix2 = (double *)malloc((end_index - start_index) * sizeof(double));
    double *local_sum = (double *)malloc((end_index - start_index) * sizeof(double));
    double *local_difference = (double *)malloc((end_index - start_index) * sizeof(double));
    double *local_product = (double *)malloc((end_index - start_index) * sizeof(double));
    double *local_quotient = (double *)malloc((end_index - start_index) * sizeof(double));

    if (!local_matrix1 || !local_matrix2 || !local_sum || !local_difference || !local_product || !local_quotient) {
        fprintf(stderr, "Ошибка выделения памяти для локальных массивов.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Рассылаем части массивов всем процессам
    MPI_Scatterv(matrix1, chunk_size + (remainder > 0), MPI_DOUBLE, local_matrix1, (end_index - start_index), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(matrix2, chunk_size + (remainder > 0), MPI_DOUBLE, local_matrix2, (end_index - start_index), MPI_DOUBLE, 0, MPI_COMM_WORLD);


    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();
    for (int i = 0; i < (end_index - start_index); i++) {
        local_sum[i] = local_matrix1[i] + local_matrix2[i];
        local_difference[i] = local_matrix1[i] - local_matrix2[i];
        local_product[i] = local_matrix1[i] * local_matrix2[i];
        local_quotient[i] = (local_matrix2[i] != 0) ? local_matrix1[i] / local_matrix2[i] : 0;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    double local_time = end_time - start_time;
    double max_time;

    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);


    // Собираем результаты обратно 
    MPI_Gatherv(local_sum, (end_index - start_index), MPI_DOUBLE, sum, chunk_size + (remainder > 0), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gatherv(local_difference, (end_index - start_index), MPI_DOUBLE, difference, chunk_size + (remainder > 0), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gatherv(local_product, (end_index - start_index), MPI_DOUBLE, product, chunk_size + (remainder > 0), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gatherv(local_quotient, (end_index - start_index), MPI_DOUBLE, quotient, chunk_size + (remainder > 0), MPI_DOUBLE, 0, MPI_COMM_WORLD);


    if (rank == 0) {
        printf("Параллельное выполнение завершено.\n");
        printf("Максимальное время выполнения: %f секунд\n", max_time);
    }


    free(local_matrix1);
    free(local_matrix2);
    free(local_sum);
    free(local_difference);
    free(local_product);
    free(local_quotient);


    if (rank == 0) {
        free(matrix1);
        free(matrix2);
        free(sum);
        free(difference);
        free(product);
        free(quotient);
    }

    MPI_Finalize();
    return 0;
}

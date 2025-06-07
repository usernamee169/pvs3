#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) printf("Usage: %s <array_size>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int arr_size = atoi(argv[1]);
    if (arr_size <= 0) {
        if (rank == 0) printf("Array size must be positive\n");
        MPI_Finalize();
        return 1;
    }

    int *arr = NULL;
    if (rank == 0) {
        arr = (int *)malloc(arr_size * sizeof(int));
        srand(time(NULL));
        for (int i = 0; i < arr_size; i++) {
            arr[i] = rand() % 1000;
        }
    }

    double start = MPI_Wtime();
    int local_size = arr_size / size;
    int *local_arr = (int *)malloc(local_size * sizeof(int));

    MPI_Scatter(arr, local_size, MPI_INT, local_arr, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    long long local_sum = 0;
    for (int i = 0; i < local_size; i++) {
        local_sum += local_arr[i];
    }

    long long global_sum;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double end = MPI_Wtime();

    if (rank == 0) {
        printf("Array sum: %lld\n", global_sum);
        printf("Time taken: %f seconds\n", end - start);
        free(arr);
    }

    free(local_arr);
    MPI_Finalize();
    return 0;
}

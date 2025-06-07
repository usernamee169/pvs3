#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0) printf("Usage: %s <rows> <cols>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    if (rows <= 0 || cols <= 0) {
        if (rank == 0) printf("Dimensions must be positive\n");
        MPI_Finalize();
        return 1;
    }

    int total_size = rows * cols;
    int local_size = total_size / size;
    int remainder = total_size % size;

    int *local_arr1 = (int *)malloc(local_size * sizeof(int));
    int *local_arr2 = (int *)malloc(local_size * sizeof(int));

    int *arr1 = NULL, *arr2 = NULL;
    if (rank == 0) {
        arr1 = (int *)malloc(total_size * sizeof(int));
        arr2 = (int *)malloc(total_size * sizeof(int));
        srand(time(NULL));
        for (int i = 0; i < total_size; i++) {
            arr1[i] = rand() % 1000 + 1;
            arr2[i] = rand() % 1000 + 1;
        }
    }

    double start = MPI_Wtime();

    MPI_Scatter(arr1, local_size, MPI_INT, local_arr1, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(arr2, local_size, MPI_INT, local_arr2, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < local_size; i++) {
        int sum = local_arr1[i] + local_arr2[i];
        int diff = local_arr1[i] - local_arr2[i];
        int prod = local_arr1[i] * local_arr2[i];
        float div = (float)local_arr1[i] / local_arr2[i];
    }

    double end = MPI_Wtime();

    if (rank == 0) {
        printf("Operations completed\n");
        printf("Time taken: %f seconds\n", end - start);
        free(arr1);
        free(arr2);
    }

    free(local_arr1);
    free(local_arr2);
    MPI_Finalize();
    return 0;
}

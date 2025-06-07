#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void bubble_sort(int *arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

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

    bubble_sort(local_arr, local_size);

    MPI_Gather(local_arr, local_size, MPI_INT, arr, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        bubble_sort(arr, arr_size);
        double end = MPI_Wtime();

        printf("First 10 elements: ");
        for (int i = 0; i < 10 && i < arr_size; i++) {
            printf("%d ", arr[i]);
        }
        printf("\nTime taken: %f seconds\n", end - start);
        free(arr);
    }

    free(local_arr);
    MPI_Finalize();
    return 0;
}

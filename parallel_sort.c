#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void print_array(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void odd_even_sort(int arr[], int n) {
    int sorted = 0;
    while (!sorted) {
        sorted = 1;

        for (int i = 0; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                swap(&arr[i], &arr[i + 1]);
                sorted = 0;
            }
        }

        for (int i = 1; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                swap(&arr[i], &arr[i + 1]);
                sorted = 0;
            }
        }

        MPI_Allreduce(MPI_IN_PLACE, &sorted, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);
    }
}


int main(int argc, char *argv[]) {
    int rank, size;
    int n;
    int *arr = NULL;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        if (argc != 2) {
            fprintf(stderr, "Использование: %s <размер_массива>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        n = atoi(argv[1]);
        if (n <= 0) {
            fprintf(stderr, "Размер массива должен быть положительным целым числом.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        arr = (int *)malloc(n * sizeof(int));
        if (arr == NULL) {
            fprintf(stderr, "Ошибка выделения памяти.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        srand(time(NULL));
        for (int i = 0; i < n; i++) {
            arr[i] = rand() % 1000;
        }
    }


    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_n = n / size;
    if (n % size != 0) {
        if (rank == 0) {
          fprintf(stderr, "Размер массива должен быть кратен количеству процессов.\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int *local_arr = (int *)malloc(local_n * sizeof(int));
    if (local_arr == NULL) {
        fprintf(stderr, "Ошибка выделения памяти на процессе %d.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Scatter(arr, local_n, MPI_INT, local_arr, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    start_time = MPI_Wtime();

    odd_even_sort(local_arr, local_n);

    end_time = MPI_Wtime();

    MPI_Gather(local_arr, local_n, MPI_INT, arr, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
       printf("Время параллельной сортировки: %f секунд,  количество процессов = %d\n", end_time - start_time, size);
       free(arr);
    }

    free(local_arr);
    MPI_Finalize();
    return 0;
}

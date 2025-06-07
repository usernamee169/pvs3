#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void array_operations(double *arr1, double *arr2, double *sum, double *diff, double *prod, double *div, int size) {
  for (int i = 0; i < size; i++) {
    sum[i] = arr1[i] + arr2[i];
    diff[i] = arr1[i] - arr2[i];
    prod[i] = arr1[i] * arr2[i];
    if (arr2[i] != 0.0) {
      div[i] = arr1[i] / arr2[i];
    } else {
      div[i] = 0.0; 
    }
  }
}

int main(int argc, char *argv[]) {
  int rank, size, array_size;
  double *arr1 = NULL, *arr2 = NULL, *sum = NULL, *diff = NULL, *prod = NULL, *div = NULL;
  double *local_arr1 = NULL, *local_arr2 = NULL, *local_sum = NULL, *local_diff = NULL, *local_prod = NULL, *local_div = NULL;
  int local_size;
  double start_time, end_time;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (argc != 2) {
    if (rank == 0) {
      fprintf(stderr, "Использование: mpirun -n <кол-во_процессов> %s <размер_массива>\n", argv[0]);
    }
    MPI_Finalize();
    return 1;
  }

  array_size = atoi(argv[1]);
  if (array_size <= 0) {
    if (rank == 0) {
      fprintf(stderr, "Размер массива должен быть положительным числом.\n");
    }
    MPI_Finalize();
    return 1;
  }

  local_size = array_size / size;
  if (array_size % size != 0) {
    if (rank == 0) {
      fprintf(stderr, "Размер массива должен быть кратен количеству процессов.\n");
    }
    MPI_Finalize();
    return 1;
  }

  if (rank == 0) {
    arr1 = (double *)malloc(array_size * sizeof(double));
    arr2 = (double *)malloc(array_size * sizeof(double));
    sum = (double *)malloc(array_size * sizeof(double));
    diff = (double *)malloc(array_size * sizeof(double));
    prod = (double *)malloc(array_size * sizeof(double));
    div = (double *)malloc(array_size * sizeof(double));

    if (!arr1 || !arr2 || !sum || !diff || !prod || !div) {
      fprintf(stderr, "Ошибка выделения памяти на процессе 0.\n");
      free(arr1); free(arr2); free(sum); free(diff); free(prod); free(div);
      MPI_Abort(MPI_COMM_WORLD, 1);
    }


    srand(time(NULL));
    for (int i = 0; i < array_size; i++) {
      arr1[i] = (double)rand() / RAND_MAX;
      arr2[i] = (double)rand() / RAND_MAX;
    }
  }

  local_arr1 = (double *)malloc(local_size * sizeof(double));
  local_arr2 = (double *)malloc(local_size * sizeof(double));
  local_sum = (double *)malloc(local_size * sizeof(double));
  local_diff = (double *)malloc(local_size * sizeof(double));
  local_prod = (double *)malloc(local_size * sizeof(double));
  local_div = (double *)malloc(local_size * sizeof(double));

  if (!local_arr1 || !local_arr2 || !local_sum || !local_diff || !local_prod || !local_div) {
    fprintf(stderr, "Ошибка выделения памяти на процессе %d.\n", rank);
    free(local_arr1); free(local_arr2); free(local_sum); free(local_diff); free(local_prod); free(local_div);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  // Рассылка данных
  MPI_Scatter(arr1, local_size, MPI_DOUBLE, local_arr1, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(arr2, local_size, MPI_DOUBLE, local_arr2, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Выполнение операций
  MPI_Barrier(MPI_COMM_WORLD);  //Синхронизация перед измерением времени
  start_time = MPI_Wtime();
  array_operations(local_arr1, local_arr2, local_sum, local_diff, local_prod, local_div, local_size);
  end_time = MPI_Wtime();
  MPI_Barrier(MPI_COMM_WORLD); // Синхронизация после измерения времени

  // Сбор результатов
  MPI_Gather(local_sum, local_size, MPI_DOUBLE, sum, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Gather(local_diff, local_size, MPI_DOUBLE, diff, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Gather(local_prod, local_size, MPI_DOUBLE, prod, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Gather(local_div, local_size, MPI_DOUBLE, div, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);


  if (rank == 0) {
    double elapsed_time = end_time - start_time;
    printf("Параллельное выполнение: Размер массива = %d, Кол-во процессов = %d, Время = %f секунд\n", array_size, size, elapsed_time);
  }

  free(local_arr1);
  free(local_arr2);
  free(local_sum);
  free(local_diff);
  free(local_prod);
  free(local_div);

  if (rank == 0) {
    free(arr1);
    free(arr2);
    free(sum);
    free(diff);
    free(prod);
    free(div);
  }

  MPI_Finalize();
  return 0;
}

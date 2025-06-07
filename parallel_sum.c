#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
  int rank, size;
  int array_size;
  int *arr = NULL;
  long long global_sum = 0;
  long long local_sum = 0;
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

  if (rank == 0) {
    array_size = atoi(argv[1]);
    if (array_size <= 0) {
      fprintf(stderr, "Размер массива должен быть положительным целым числом.\n");
      MPI_Abort(MPI_COMM_WORLD, 1);
      MPI_Finalize();
      return 1;
    }
  }

  // Рассылка размера массива всем процессам
  MPI_Bcast(&array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Вычисление размера локального массива для каждого процесса
  int local_size = array_size / size;
  int remainder = array_size % size;
  if (rank < remainder) {
    local_size++;
  }


  int *local_arr = (int *)malloc(local_size * sizeof(int));
  if (local_arr == NULL) {
    perror("Ошибка выделения памяти");
    MPI_Abort(MPI_COMM_WORLD, 1);
    MPI_Finalize();
    return 1;
  }


  if (rank == 0) {
    arr = (int *)malloc(array_size * sizeof(int));
    if (arr == NULL) {
      perror("Ошибка выделения памяти");
      MPI_Abort(MPI_COMM_WORLD, 1);
      MPI_Finalize();
      return 1;
    }
    srand(time(NULL)); 
    for (int i = 0; i < array_size; i++) {
      arr[i] = rand() % 100; 
    }
  }

  // Рассылка частей массива по процессам
  int *sendcounts = (int*)malloc(size * sizeof(int));
  int *displs = (int*)malloc(size * sizeof(int));
  int offset = 0;
  for (int i = 0; i < size; i++) {
        sendcounts[i] = array_size / size;
        if (i < array_size % size) {
            sendcounts[i]++;
        }
        displs[i] = offset;
        offset += sendcounts[i];
    }

  MPI_Scatterv(arr, sendcounts, displs, MPI_INT,
               local_arr, local_size, MPI_INT,
               0, MPI_COMM_WORLD);

    if(rank == 0){
        free(arr);
        free(sendcounts);
        free(displs);
    }

  // Вычисление локальной суммы
  start_time = MPI_Wtime();
  for (int i = 0; i < local_size; i++) {
    local_sum += local_arr[i];
  }
  end_time = MPI_Wtime();

  // Сбор всех локальных сумм в глобальную сумму
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    printf("Сумма элементов массива: %lld\n", global_sum);
    printf("Время выполнения: %f секунд\n", end_time - start_time);
  }

  free(local_arr);

  MPI_Finalize();
  return 0;
}

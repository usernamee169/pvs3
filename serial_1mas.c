#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
  if (argc != 2) {
    fprintf(stderr, "Использование: %s <размер_массива>\n", argv[0]);
    return 1;
  }

  int size = atoi(argv[1]);
  if (size <= 0) {
    fprintf(stderr, "Размер массива должен быть положительным числом.\n");
    return 1;
  }

  double *arr1 = (double *)malloc(size * sizeof(double));
  double *arr2 = (double *)malloc(size * sizeof(double));
  double *sum = (double *)malloc(size * sizeof(double));
  double *diff = (double *)malloc(size * sizeof(double));
  double *prod = (double *)malloc(size * sizeof(double));
  double *div = (double *)malloc(size * sizeof(double));

  if (!arr1 || !arr2 || !sum || !diff || !prod || !div) {
    fprintf(stderr, "Ошибка выделения памяти.\n");
    free(arr1); free(arr2); free(sum); free(diff); free(prod); free(div);
    return 1;
  }


  srand(time(NULL));
  for (int i = 0; i < size; i++) {
    arr1[i] = (double)rand() / RAND_MAX; 
    arr2[i] = (double)rand() / RAND_MAX; 
  }

  clock_t start_time = clock();
  array_operations(arr1, arr2, sum, diff, prod, div, size);
  clock_t end_time = clock();

  double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
  printf("Последовательное выполнение: Размер массива = %d, Время = %f секунд\n", size, elapsed_time);


  free(arr1);
  free(arr2);
  free(sum);
  free(diff);
  free(prod);
  free(div);

  return 0;
}

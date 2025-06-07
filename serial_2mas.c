#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <rows> <cols>\n", argv[0]);
        return 1;
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    if (rows <= 0 || cols <= 0 || (long)rows * cols < 100000) {
        fprintf(stderr, "Размеры массивов должны быть положительными и общее количество элементов должно быть не менее 100000.\n");
        return 1;
    }

    double *matrix1 = (double *)malloc(rows * cols * sizeof(double));
    double *matrix2 = (double *)malloc(rows * cols * sizeof(double));
    double *sum = (double *)malloc(rows * cols * sizeof(double));
    double *difference = (double *)malloc(rows * cols * sizeof(double));
    double *product = (double *)malloc(rows * cols * sizeof(double));
    double *quotient = (double *)malloc(rows * cols * sizeof(double));

    if (!matrix1 || !matrix2 || !sum || !difference || !product || !quotient) {
        fprintf(stderr, "Ошибка выделения памяти.\n");
        return 1;
    }

    srand(time(NULL));
    for (int i = 0; i < rows * cols; i++) {
        matrix1[i] = (double)rand() / RAND_MAX;
        matrix2[i] = (double)rand() / RAND_MAX; 
    }

    clock_t start = clock();
    for (int i = 0; i < rows * cols; i++) {
        sum[i] = matrix1[i] + matrix2[i];
        difference[i] = matrix1[i] - matrix2[i];
        product[i] = matrix1[i] * matrix2[i];
        quotient[i] = (matrix2[i] != 0) ? matrix1[i] / matrix2[i] : 0;
    }
    clock_t end = clock();

    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;


    printf("Время выполнения: %f секунд\n", time_spent);

    free(matrix1);
    free(matrix2);
    free(sum);
    free(difference);
    free(product);
    free(quotient);

    return 0;
}

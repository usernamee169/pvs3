#include <iostream>
#include <vector>
#include <cstdlib>
#include <mpi.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            cerr << "Использование: " << argv[0] << " <размер_массива>" << endl;
        }
        MPI_Finalize();
        return 1;
    }

    int array_size = atoi(argv[1]);

    if (array_size <= 0) {
        if (rank == 0) {
            cerr << "Размер массива должен быть положительным целым числом." << endl;
        }
        MPI_Finalize();
        return 1;
    }

    vector<int> arr;
    int local_size;
    vector<int> local_arr;

    if (rank == 0) {
        arr.resize(array_size);
        for (int i = 0; i < array_size; ++i) {
            arr[i] = rand() % 100;
        }
    }

    // Рассчитываем размер локального массива для каждого процесса
    local_size = array_size / size;
    if (array_size % size != 0 && rank == size - 1) {
        local_size = array_size - (size - 1) * (array_size / size);
    }

    local_arr.resize(local_size);

    // Рассылаем части массива каждому процессу
    MPI_Scatter(arr.data(), array_size / size, MPI_INT, local_arr.data(), array_size / size, MPI_INT, 0, MPI_COMM_WORLD);
    if (array_size % size != 0 && rank == size - 1) {
        MPI_Send(arr.data() + (size - 1) * (array_size / size), array_size - (size - 1) * (array_size / size), MPI_INT, rank, 0, MPI_COMM_WORLD);
        MPI_Status status;
        MPI_Recv(local_arr.data(), array_size - (size - 1) * (array_size / size), MPI_INT, rank, 0, MPI_COMM_WORLD, &status);
    }

    auto start = high_resolution_clock::now();

    long long local_sum = 0;
    for (int i = 0; i < local_size; ++i) {
        local_sum += local_arr[i];
    }

    long long global_sum = 0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    if (rank == 0) {
        cout << "Параллельная сумма: " << global_sum << endl;
        cout << "Время выполнения: " << duration.count() << " микросекунд" << endl;
    }

    MPI_Finalize();
    return 0;
}

#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>

using namespace std;
using namespace std::chrono;

void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Использование: " << argv[0] << " <размер_массива>" << endl;
        return 1;
    }

    int array_size = atoi(argv[1]);

    if (array_size <= 0) {
        cerr << "Размер массива должен быть положительным целым числом." << endl;
        return 1;
    }

    vector<int> arr(array_size);
    for (int i = 0; i < array_size; ++i) {
        arr[i] = rand() % 100;
    }

    auto start = high_resolution_clock::now();

    bubbleSort(arr);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Время выполнения: " << duration.count() << " микросекунд" << endl;

    return 0;
}

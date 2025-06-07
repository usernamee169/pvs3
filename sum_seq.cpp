#include <iostream>
#include <vector>
#include <cstdlib> 
#include <chrono> 

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Использование: " << argv[0] << " <размер_массива>" << endl;
        return 1;
    }

    int array_size = atoi(argv[1]); // Получаем размер массива из аргумента командной строки

    if (array_size <= 0) {
        cerr << "Размер массива должен быть положительным целым числом." << endl;
        return 1;
    }

    vector<int> arr(array_size);
  
    for (int i = 0; i < array_size; ++i) {
        arr[i] = rand() % 100; 
    }

    auto start = high_resolution_clock::now(); 

    long long sum = 0;
    for (int i = 0; i < array_size; ++i) {
        sum += arr[i];
    }

    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Последовательная сумма: " << sum << endl;
    cout << "Время выполнения: " << duration.count() << " микросекунд" << endl;

    return 0;
}

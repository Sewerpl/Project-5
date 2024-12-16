#include <iostream>
#include <cmath>
#include <pthread.h>
#include <vector>
#include <chrono>

// Struktura przechowująca dane dla każdego wątku
struct ThreadData {
    long start;
    long end;
    double step;
    double partial_sum;
};

// Funkcja obliczeniowa wykonywana przez wątki
void* calculate_partial_sum(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    double sum = 0.0;

    for (long i = data->start; i < data->end; ++i) {
        double x = (i + 0.5) * data->step;
        sum += sqrt(1.0 - x * x);
    }

    data->partial_sum = sum;
    return nullptr;
}


    return 0;
}

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

int main() {
    // Parametry całkowania
    long num_intervals;   // Liczba podprzedziałów
    int num_threads;      // Liczba wątków

    // Wprowadzenie danych od użytkownika
    std::cout << "Podaj liczbę podprzedziałów (np. 100000000): ";
    std::cin >> num_intervals;
    std::cout << "Podaj liczbę wątków (np. 4): ";
    std::cin >> num_threads;

    if (num_threads <= 0 || num_intervals <= 0) {
        std::cerr << "Liczba wątków i podprzedziałów musi być większa od zera." << std::endl;
        return 1;
    }

    double step = 1.0 / num_intervals;
    std::vector<ThreadData> thread_data(num_threads);
    std::vector<pthread_t> threads(num_threads);


    return 0;
}

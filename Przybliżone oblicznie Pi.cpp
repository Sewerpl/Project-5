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

    // Podział pracy na wątki
    long chunk_size = num_intervals / num_threads;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? num_intervals : (i + 1) * chunk_size;
        thread_data[i].step = step;
        thread_data[i].partial_sum = 0.0;

        if (pthread_create(&threads[i], nullptr, calculate_partial_sum, &thread_data[i]) != 0) {
            std::cerr << "Błąd podczas tworzenia wątku." << std::endl;
            return 1;
        }
    }

    // Oczekiwanie na zakończenie pracy wątków
    double total_sum = 0.0;
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
        total_sum += thread_data[i].partial_sum;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    // Obliczenie przybliżonej wartości PI
    double pi = 4.0 * step * total_sum;

    // Wyświetlenie wyników
    std::cout << "Przybliżona wartość PI: " << pi << std::endl;
    std::cout << "Czas obliczeń: " << elapsed_time.count() << " sekund" << std::endl;

    return 0;
}

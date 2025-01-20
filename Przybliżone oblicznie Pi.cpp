#include <iostream>
#include <cmath>
#include <pthread.h>
#include <vector>
#include <chrono>

/**
 * @struct ThreadData
 * @brief Struktura przechowująca dane dla każdego wątku obliczeniowego.
 * 
 * Ta struktura zawiera dane dotyczące zakresu obliczeń, kroku całkowania
 * oraz wyniku częściowego obliczenia, które są używane przez wątki.
 */
struct ThreadData {
    long start; ///< Początkowy indeks dla zakresu obliczeń w tym wątku.
    long end;   ///< Końcowy indeks dla zakresu obliczeń w tym wątku.
    double step;///< Krok całkowania (odległość między kolejnymi punktami całkowania).
    double partial_sum; ///< Częściowy wynik obliczeń wątków (suma wartości funkcji na tym zakresie).
};

/**
 * @brief Funkcja obliczeniowa wykonywana przez wątki.
 * 
 * Funkcja ta oblicza częściową sumę w ramach określonego przedziału
 * podzielonego między wątki, wykorzystując metodę prostokątów dla
 * obliczenia całki. Każdy wątek wykonuje obliczenia dla swojego zakresu
 * indeksów, sumując wartości funkcji sqrt(1 - x^2) dla kolejnych
 * punktów na tym przedziale. 
 * 
 * Algorytm zakłada, że przedział [0, 1] jest podzielony na `num_intervals` 
 * podprzedziałów, a krok całkowania jest równy `1.0 / num_intervals`.
 *
 * @param arg Wskaźnik do struktury `ThreadData`, zawierającej dane dla wątku.
 * @return Zwraca wskaźnik `nullptr`, ponieważ wątki nie zwracają żadnej wartości.
 * 
 * @note Funkcja ta jest wykonywana równolegle w ramach kilku wątków.
 */
void* calculate_partial_sum(void* arg) {
    ThreadData* data = (ThreadData*)arg; ///< Pobranie danych wątku
    double sum = 0.0; ///< Zmienna do przechowywania częściowej sumy obliczeń wątku

    /**
     * Obliczenia całki na danym przedziale. Funkcja sqrt(1 - x^2) jest
     * obliczana dla punktów x = (i + 0.5) * step, gdzie 'i' to indeks
     * danego przedziału, a 'step' to krok całkowania.
     */
    for (long i = data->start; i < data->end; ++i) {
        double x = (i + 0.5) * data->step; ///< Obliczenie współrzędnej X dla prostokąta
        sum += sqrt(1.0 - x * x); ///< Obliczenie wartości funkcji dla punktu x
    }

    data->partial_sum = sum; ///< Przechowanie częściowego wyniku dla tego wątku
    return nullptr; ///< Wątek kończy działanie
}

/**
 * @brief Funkcja główna programu obliczającego wartość liczby PI przy użyciu metody prostokątów.
 * 
 * Program oblicza przybliżoną wartość liczby PI, dzieląc całkowanie na wiele wątków. 
 * Każdy wątek oblicza część całki, a wyniki są sumowane, aby uzyskać końcowy wynik. 
 * Program przyjmuje dwie wartości wejściowe: liczbę podprzedziałów oraz liczbę wątków, 
 * a następnie oblicza wynik równolegle.
 * 
 * Całkowanie jest realizowane za pomocą metody prostokątów, gdzie funkcja
 * sqrt(1 - x^2) jest całkowana na przedziale [0, 1].
 * 
 * @return Zwraca 0, jeśli program zakończył się poprawnie, lub 1 w przypadku błędów.
 * 
 * @note Wartość PI jest przybliżona na podstawie metody prostokątów, 
 *       a obliczenia są przyspieszane poprzez wielowątkowość.
 */
int main() {
    long num_intervals;   ///< Liczba podprzedziałów, na które dzielimy przedział [0, 1]
    int num_threads;      ///< Liczba wątków, na które dzielimy obliczenia

    // Wprowadzenie danych od użytkownika
    std::cout << "Podaj liczbę podprzedziałów (np. 100000000): ";
    std::cin >> num_intervals;
    std::cout << "Podaj liczbę wątków (np. 4): ";
    std::cin >> num_threads;

    // Walidacja poprawności danych wejściowych
    if (num_threads <= 0 || num_intervals <= 0) {
        std::cerr << "Liczba wątków i podprzedziałów musi być większa od zera." << std::endl;
        return 1; ///< Zakończenie programu z błędem
    }

    double step = 1.0 / num_intervals; ///< Krok całkowania (odległość między kolejnymi punktami)
    std::vector<ThreadData> thread_data(num_threads); ///< Tablica struktur danych dla wątków
    std::vector<pthread_t> threads(num_threads); ///< Tablica wątków

    // Obliczenie rozmiaru części pracy przypadającej na jeden wątek
    long chunk_size = num_intervals / num_threads; ///< Rozmiar części zadania dla wątków

    // Rozpoczęcie pomiaru czasu obliczeń
    auto start_time = std::chrono::high_resolution_clock::now();

    /**
     * Tworzenie wątków. Każdy wątek oblicza część sumy całkowania w swoim zakresie
     * przedziału [start, end]. Zakresy są dzielone równomiernie między wszystkie wątki.
     */
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].start = i * chunk_size; ///< Początek zakresu dla wątku
        thread_data[i].end = (i == num_threads - 1) ? num_intervals : (i + 1) * chunk_size; ///< Koniec zakresu dla wątku
        thread_data[i].step = step; ///< Przekazanie kroku całkowania do danych wątku
        thread_data[i].partial_sum = 0.0; ///< Inicjalizacja częściowego wyniku wątku

        /**
         * Tworzenie wątku i przypisanie mu funkcji do wykonania
         * Wątek będzie wykonywał obliczenia w funkcji `calculate_partial_sum`.
         */
        if (pthread_create(&threads[i], nullptr, calculate_partial_sum, &thread_data[i]) != 0) {
            std::cerr << "Błąd podczas tworzenia wątku." << std::endl;
            return 1; ///< Zakończenie programu z błędem
        }
    }

    // Oczekiwanie na zakończenie pracy wątków
    double total_sum = 0.0; ///< Zmienna przechowująca sumę wyników ze wszystkich wątków
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr); ///< Oczekiwanie na zakończenie każdego wątku
        total_sum += thread_data[i].partial_sum; ///< Sumowanie wyników częściowych
    }

    // Zakończenie pomiaru czasu obliczeń
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    /**
     * Obliczenie przybliżonej wartości liczby PI. Całkowita suma jest przemnażana przez 4
     * oraz przez krok całkowania, aby uzyskać wynik przybliżony do wartości PI.
     */
    double pi = 4.0 * step * total_sum;

    // Wyświetlenie wyników
    std::cout << "Przybliżona wartość PI: " << pi << std::endl;
    std::cout << "Czas obliczeń: " << elapsed_time.count() << " sekund" << std::endl;

    return 0; ///< Zakończenie programu bez błędów
}

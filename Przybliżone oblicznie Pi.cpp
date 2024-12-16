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



    return 0;
}

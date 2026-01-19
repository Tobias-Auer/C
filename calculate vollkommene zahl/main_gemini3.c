#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <omp.h> // Header für Multi-Core Support

int main() {
    const uint32_t limit = 200000; // Höheres Limit zum Testen
    uint32_t *sum_array = (uint32_t *)calloc(limit, sizeof(uint32_t));
    if (!sum_array) return 1;

    double start_time = omp_get_wtime();

    // Parallelisierung des Siebs
    // 'atomic' verhindert, dass Kerne gleichzeitig auf die gleiche Speicherzelle schreiben
    #pragma omp parallel for
    for (uint32_t i = 1; i <= limit / 2; i++) {
        for (uint32_t j = 2 * i; j < limit; j += i) {
            #pragma omp atomic
            sum_array[j] += i;
        }
    }

    // Parallelisierung der Auswertung
    #pragma omp parallel for
    for (uint32_t i = 2; i < limit; i++) {
        uint32_t s1 = sum_array[i];
        if (s1 == i) {
            // printf ist nicht thread-sicher, daher 'critical' (nur ein Kern gleichzeitig)
            #pragma omp critical
            printf("Vollkommen: %u\n", i);
        } else if (s1 > i && s1 < limit) {
            if (sum_array[s1] == i) {
                #pragma omp critical
                printf("Befreundet: %u, %u\n", i, s1);
            }
        }
    }

    double end_time = omp_get_wtime();
    printf("Berechnung auf %d Kernen abgeschlossen.\n", omp_get_max_threads());
    printf("Zeit: %f sek\n", end_time - start_time);

    free(sum_array);
    return 0;
}

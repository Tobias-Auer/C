#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <omp.h>

#define LIMIT 4000000000ULL
#define CHUNK_SIZE 65536  // Passt perfekt in den L2-Cache

// Funktion zur Berechnung der Teilersumme via Primfaktoren
uint64_t get_sigma(uint64_t n, uint32_t* primes, int prime_count) {
    uint64_t total_sum = 1;
    uint64_t temp_n = n;

    for (int i = 0; i < prime_count; i++) {
        uint32_t p = primes[i];
        if ((uint64_t)p * p > temp_n) break;

        if (temp_n % p == 0) {
            uint64_t p_pow_sum = 1;
            uint64_t p_pow = 1;
            while (temp_n % p == 0) {
                p_pow *= p;
                p_pow_sum += p_pow;
                temp_n /= p;
            }
            total_sum *= p_pow_sum;
        }
    }
    if (temp_n > 1) {
        total_sum *= (temp_n + 1);
    }
    return total_sum - n; // Echte Teilersumme
}

int main() {
    // 1. Primzahlen bis sqrt(LIMIT) vorbereiten
    uint32_t sqrt_limit = (uint32_t)sqrt(LIMIT);
    uint32_t* primes = malloc(sqrt_limit * sizeof(uint32_t));
    char* is_prime = calloc(sqrt_limit + 1, 1);
    int p_count = 0;

    for (uint32_t p = 2; p <= sqrt_limit; p++) {
        if (!is_prime[p]) {
            primes[p_count++] = p;
            for (uint32_t i = p * p; i <= sqrt_limit; i += p) is_prime[i] = 1;
        }
    }
    free(is_prime);

    printf("Starting search up to %llu using %d threads...\n", LIMIT, omp_get_max_threads());
    double start = omp_get_wtime();

    // 2. Segmentierte Verarbeitung
    #pragma omp parallel
    {
        // Jeder Thread hat seinen eigenen kleinen Buffer für Ergebnisse
        // um "False Sharing" im Cache zu vermeiden.
        #pragma omp for schedule(dynamic)
        for (uint64_t chunk_start = 2; chunk_start < LIMIT; chunk_start += CHUNK_SIZE) {
            uint64_t chunk_end = chunk_start + CHUNK_SIZE;
            if (chunk_end > LIMIT) chunk_end = LIMIT;

            for (uint64_t i = chunk_start; i < chunk_end; i++) {
                uint64_t s1 = get_sigma(i, primes, p_count);
                
                if (s1 == i) {
                    #pragma omp critical
                    printf("\nFound Perfect: %lu", i);
                } else if (s1 > i && s1 < LIMIT) {
                    // Checke das Gegenstück
                    if (get_sigma(s1, primes, p_count) == i) {
                        #pragma omp critical
                        printf("\nFound Amicable Pair: %lu and %lu", i, s1);
                    }
                }
            }
        }
    }

    double end = omp_get_wtime();
    printf("\nFinished in %f seconds.\n", end - start);
    
    free(primes);
    return 0;
}

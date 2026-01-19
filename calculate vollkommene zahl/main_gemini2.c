#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Wir nutzen uint32_t für garantierte 32-Bit (0 bis 4,2 Mrd)
#include <stdint.h>

int main() {
    const uint32_t limit = 200000;
    
    // Wir reservieren Speicher für alle Summen bis 'limit'
    // calloc initialisiert alles mit 0
    uint32_t *sum_array = (uint32_t *)calloc(limit, sizeof(uint32_t));
    if (!sum_array) return 1;

    clock_t begin = clock();

    // DAS SIEB: Wir addieren jeden Teiler i zu all seinen Vielfachen
    // Das ist massiv schneller als Modulo-Operationen!
    for (uint32_t i = 1; i <= limit / 2; i++) {
        for (uint32_t j = 2 * i; j < limit; j += i) {
            sum_array[j] += i;
        }
    }

    // Auswertung (geht jetzt blitzschnell, da nur noch Array-Lookups)
    FILE *fptr = fopen("data.txt", "w");
    for (uint32_t i = 2; i < limit; i++) {
        uint32_t s1 = sum_array[i];
        
        if (s1 == i) {
            printf("Vollkommen: %u\n", i);
        } else if (s1 > i && s1 < limit) {
            if (sum_array[s1] == i) {
                printf("Befreundet: %u, %u\n", i, s1);
                fprintf(fptr, "%u, %u\n", i, s1);
            }
        }
    }

    clock_t end = clock();
    printf("Zeit: %f sek\n", (double)(end - begin) / CLOCKS_PER_SEC);

    fclose(fptr);
    free(sum_array);
    return 0;
}

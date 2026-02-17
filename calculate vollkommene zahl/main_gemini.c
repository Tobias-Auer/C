#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
const unsigned long int max = 0-1;

// Hilfsfunktion: Berechnet direkt die Summe der echten Teiler
unsigned long sum_divisors(unsigned long n) {
    if (n < 2) return 0;
    unsigned long sum = 1; // 1 ist immer ein Teiler
    
    // Wir suchen nur bis zur Quadratwurzel
    unsigned long root = (unsigned long)sqrt(n);
    
    for (unsigned long i = 2; i <= root; i++) {
        if (n % i == 0) {
            sum += i;
            unsigned long pair = n / i;
            if (pair != i) {
                sum += pair;
            }
        }
    }
    return sum;
}

int main() {
    clock_t begin = clock();
    FILE *fptr = fopen("data.txt", "w"); // "w" statt "a", um Datei neu zu schreiben
    
    if (fptr == NULL) {
        printf("FEHLER BEIM ÖFFNEN DER DATEI\n");
        return 1;
    }

    printf("Suche bis: %lu\n", max);

    for (unsigned long i = 2; i < max; i++) {
        unsigned long sum1 = sum_divisors(i);
        
        // Befreundete Zahlen: sum1 muss größer als i sein, 
        // damit wir jedes Paar nur EINMAL finden und ausgeben.
        if (sum1 > i && sum1 < 4000000000UL) { 
            unsigned long sum2 = sum_divisors(sum1);
            if (sum2 == i) {
                printf("FOUND BEFREUNDET: %lu, %lu\n", i, sum1);
                fprintf(fptr, "%lu, %lu\n", i, sum1);
            }
        } 
        // Vollkommene Zahlen: sum1 ist gleich der Zahl selbst
        else if (sum1 == i) {
            printf("FOUND VOLLKOMMEN: %lu\n", i);
            fprintf(fptr, "Vollkommen: %lu\n", i);
        }
    }

    fclose(fptr);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("ZEIT: %f Sekunden\n", time_spent);
    
    return 0;
}

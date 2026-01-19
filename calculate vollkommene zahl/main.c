#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
const unsigned long int max = 200000;
unsigned long int foundPairs[9999][2];
int foundPairsIndex = 0;
unsigned long int arrayIndex = 0;
unsigned long int currentNumbers[9999];


bool find_vollkommen = true;
bool find_befreundet = true;
unsigned long int return_sum(unsigned long int list[], unsigned long int length);
void findPair(unsigned long int testNumber);

void set_true_dividers(unsigned long int target); 

FILE *fptr;

int main() {
	clock_t begin = clock();
	fptr = fopen("data.txt","a");
	if (fptr == NULL) {
		printf("FAILED TO OPEN SAVEFILE");
		exit(1);
	}
	char str[64];
	sprintf(str, "Suche nach vollkommenen Zahlen bis: %lu\n", max);
	printf(str);

	for (unsigned long int i = 1; i<max; i++) {
		findPair(i);
	}
	fclose(fptr);
	clock_t end = clock();
	double time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
	char time[64];
	sprintf(time, "TIME: %g\n", time_spent);
	printf(time);
	return 0;
}

unsigned long int return_sum(unsigned long int list[], unsigned long int length) {
	unsigned long long int sum = 0;
	for (unsigned long int i=0; i<length; i++) {
		if (list[i] == 0) {
			break;
		}
		sum += list[i];
		
	}
	return sum;
}

void set_true_dividers(unsigned long int target) {
	memset(currentNumbers, 0, sizeof(currentNumbers));		
	arrayIndex = 0;
	for (unsigned long int i = 1; i<target;i++) {
		if (target%i==0) {
			currentNumbers[arrayIndex] = i;
			arrayIndex++;
		}
	}
}


void findPair(unsigned long int testNumber) {
	set_true_dividers(testNumber);
	
	unsigned long int sum1 = return_sum(currentNumbers, arrayIndex);

	set_true_dividers(sum1);
	unsigned long int sum2 = return_sum(currentNumbers, arrayIndex);
	if (testNumber == sum2) {
		if (sum1==sum2 && !find_vollkommen) {
			return;
		}
		printf("FOUND: %lu, %lu\n", sum1, sum2);
		foundPairs[foundPairsIndex][0] = sum1;
		foundPairs[foundPairsIndex][1] = sum2;
		foundPairsIndex++;
		char str1[sizeof(sum1)];
		char str2[sizeof(sum2)];
		sprintf(str1, "%lu, ", sum1);
		fprintf(fptr, str1);
		
		sprintf(str2, "%lu\n", sum2);
		fprintf(fptr, str2);
		
	}
}

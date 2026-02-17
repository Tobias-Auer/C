#include<stdio.h>

int calc(int zahl1, int zahl2) {
	int r0 = zahl2, r1 = zahl1;  // reste
	int t0 = 0, t1 = 1;          // koeffizienten für zahl1 (wird am ende inverse)
	
	while (r1 != 0) {
		int q = r0/r1;
		int r2 = r0%r1;

		int t2 = t0 - q*t1;

		r0 = r1;
		r1 = r2;
		t0 = t1;
		t1 = t2;
	}
	if (r0 != 1) return -1;

	int inverse = t0 % zahl2;
	if (inverse<0) inverse+=zahl2;
	
	return inverse;
}

int main() {
	int x;
	int y;
	printf("Hallo, berechnen von Inversen\nFormat: x mod y\n");
	printf("Gebe die erste Zahl(x) ein: ");
	scanf("%d", &x);
	printf("Gebe die zweite Zahl(y) ein: ");
	scanf("%d", &y);
	int calcResult = calc(x,y);
	printf("Die Inverse ist: %d\n", calcResult);
	return 0;
}


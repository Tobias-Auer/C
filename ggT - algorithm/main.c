#include<stdio.h>

int x;
int y;

int calc(int zahl1, int zahl2) {
	int result = zahl1 % zahl2;
	if (result == 0) {
		return zahl2;
	}
	return calc(zahl2, result);
}

int main() {
	printf("Hallo\n");
	printf("Gebe die erste Zahl ein: ");
	scanf("%d", &x);
	printf("Gebe die zweite Zahl ein: ");
	scanf("%d", &y);
	int calcResult = calc(x,y);
	printf("Der ggT ist: %d\n", calcResult);
	return 0;
}


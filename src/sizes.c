#include <stdio.h>

int main(int argc, char *argv[]) {
	printf("c=%lu s=%lu i=%lu l=%lu h=%lu f=%lu d=%lu x=%lu p=%lu", sizeof(char), sizeof(short), sizeof(int), sizeof(long), sizeof(long long), sizeof(float), sizeof(double), sizeof(long double), sizeof(char*));
	return 0;
}
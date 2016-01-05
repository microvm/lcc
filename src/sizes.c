#include <stdio.h>

int main(int argc, char *argv[]) {
	printf("c=%zu s=%zu i=%zu l=%zu h=%zu f=%zu d=%zu x=%zu p=%zu", sizeof(char), sizeof(short), sizeof(int), sizeof(long), sizeof(long long), sizeof(float), sizeof(double), sizeof(long double), sizeof(char*));
	return 0;
}
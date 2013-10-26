/*
 * clang generics.c -o generics -std=c11
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define sqr(x) _Generic((x), double: sqrd, unsigned int: sqri, default: sqri)(x)

unsigned int sqri(unsigned int x) {
	fprintf(stderr, "Called sqri.\n");
	return x * x;
}

double sqrd(double x) {
	fprintf(stderr, "Called sqrd.\n");
	return (double) (x * x);
}

int main(int argc, char *argv[]) {
	unsigned int a = 4 * 1000 * 1000;
	double b = ((double)(INT_MAX)) + 5.0;

	fprintf(stdout, "%f\n", sqr(b));
	fprintf(stdout, "%u\n", sqr(a));

	return 0;
}

/*
 * Trying new strcpy_s of C11.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(int argc, char *argv[]) {
	char arr1[] = "Hello.";
	char arr2[] = { 'W', 'o', 'r', 'l', 'd', '\0' };

	char arr3[20], arr4[20];

	rsize_t r;

	r = strcpy_s(arr3, sizeof (arr3), arr1);
	assert(r == 0);
}


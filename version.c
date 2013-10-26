/**
 * @author Vaibhav Bhembre
 * @version 2013/09/28
 * gcc version.c -o version -std=c99
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    printf("C version: %ld\n", __STDC_VERSION__);
    return 0;
}


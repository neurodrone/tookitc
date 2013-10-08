/**
 * @author Vaibhav Bhembre
 * @version 2013/10/08
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void get_cpuid(int val[static 4], uint32_t in) {
    /* Compiling with Clang enforces the array size to be 4 here.
     * Passing in a pointer to an array of less or more elements
     * or NULL will fail. Sadly, GCC doesn't care.
     */
    __asm__ __volatile__ (
            "cpuid"
            : "=a" (val[0])
            , "=b" (val[1])
            , "=c" (val[2])
            , "=d" (val[3])
            : "a" (in)
            : "memory");
}

void get_cpu_addrsize(uint8_t pl[static 2]) {
    int val[4] = { 0 };

    get_cpuid(val, 0x80000008);

    pl[0] = val[0] & 0xFF;
    pl[1] = (val[0] >> 8) & 0xFF;
}

int main(int argc, char *argv[]) {
    uint8_t pl[2];
    get_cpu_addrsize(pl);

    printf("Physical address: %u\n", pl[0]);
    printf("Linear address: %u\n", pl[1]);

    return 0;
}


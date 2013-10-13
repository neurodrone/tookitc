/**
 * @author Vaibhav Bhembre
 * @version 2013/10/08
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

#define AES_BIT 25

enum {
    EAX,
    EBX,
    ECX,
    EDX,
    ELEN
};

enum {
    PHY,
    LIN,
    PLSIZE
};

static int val[ELEN];

void get_cpuid(int val[static ELEN], uint32_t in) {
    /* Compiling with Clang enforces the array size to be 4 here.
     * Passing in a pointer to an array of less or more elements
     * or NULL will fail. Sadly, GCC doesn't care.
     */
    __asm__ __volatile__ (
            "cpuid"
            : "=a" (val[EAX])
            , "=b" (val[EBX])
            , "=c" (val[ECX])
            , "=d" (val[EDX])
            : "a" (in)
            : "memory");
}

void get_cpu_addrsize(uint8_t pl[static PLSIZE]) {
    get_cpuid(val, (1 << 31) | (1 << 3));

    pl[PHY] = val[EAX] & 0xFF;
    pl[LIN] = (val[EAX] >> CHAR_BIT) & 0xFF;
}

void get_cpu_brand(char **brand) {
    get_cpuid(val, 0);

    /* Quick optimized swap */
    __asm__ __volatile__ (
            "xchgl %0, %1"
            : "=a" (val[ECX]), "=b" (val[EDX])
            : "0" (val[ECX]), "1" (val[EDX])
    );

    *brand = (char *) &val[EBX];
}

bool has_aes() {
    get_cpuid(val, 1);

    return val[ECX] & (1 << AES_BIT);
    /* Bit reference from http://intel.ly/17VZqYA (Pg. 3-168)*/
}

int main(int argc, char *argv[]) {
    uint8_t pl[PLSIZE];
    get_cpu_addrsize(pl);

    printf("Physical address: %u\n", pl[PHY]);
    printf("Linear address: %u\n", pl[LIN]);

    char *brand;
    get_cpu_brand(&brand);
    printf("CPU: %s\n", brand);

    printf("AES NativeIns: %s\n", (has_aes() ? "ENABLED" : "DISABLED"));

    return 0;
}


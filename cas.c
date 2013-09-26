/**
 * @author Vaibhav Bhembre
 * @version 2013/09/26
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool _cas(int *, int, int);

int main(int argc, char *argv[]) {
    int *p = alloca(sizeof (int));
    int oldValue = 5, newValue = 10;
    bool ret;
    
    *p = oldValue;

    printf("Value at `p`: %d\n", *p);
    ret = _cas(p, oldValue, newValue);
    printf("Value at `p`: %d\n", *p);

    return 0;
}

bool _cas(int *p, int oldValue, int newValue) {
    bool res;
    __asm__ __volatile__ (
            "lock; cmpxchgl %2, %0;"
            "setz %1"
            : "=m" (*p), "=a" (res)
            : "q" (newValue), "a" (oldValue)
            : "memory", "cc");
    return res;
}


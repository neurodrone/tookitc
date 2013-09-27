/**
 * @author Vaibhav Bhembre
 * @version 2013/09/26
 */

#include <stdio.h>
#include <stdlib.h>

int _faa(int *, int);

int main(int argc, char *argv[]) {
    int *p = alloca(sizeof (*p)), incr = 10, oldValue;

    *p = 5;
    printf("Value at `p`: %d\n", *p);
    oldValue = _faa(p, incr);
    printf("OldValue at `p`: %d, Value at `p`: %d\n", oldValue, *p);

    int decr = -10;
    oldValue = _faa(p, decr);
    printf("OldValue at `p`: %d, Value at `p`: %d\n", oldValue, *p);

    return 0;
}

int _faa(int *p, int incr) {
    __asm__ __volatile__ (
            "lock; xaddl %1, %0;"
            : "+m" (*p), "+q" (incr)
            :
            : "memory", "cc");
    return incr;
}

/* Output: 
 *
 *  Value at `p`: 5
 *  OldValue at `p`: 5, Value at `p`: 15
 *  OldValue at `p`: 15, Value at `p`: 5
*/


/**
 * @author Vaibhav Bhembre
 * @version 2013/10/06
 */

#include <stdio.h>
#include <stdlib.h>

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
/* !!(x) translates 'x' into `true` or `false`. */

int main(int argc, char *argv[]) {
    int a = 5;

    if (argc == 2) {
        a = atoi(argv[1]);
    }

    if (unlikely(a == 2)) {
        a++;
    } else {
        a--;
    }

    printf("a: %d\n", a);

    return 0;
}


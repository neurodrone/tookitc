/**
 * @author Vaibhav Bhembre
 * @version 2013/10/16
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void profile_func_enter(void *this_fn, void *call_site)__attribute__((no_instrument_function));
void profile_func_exit(void *this_fn, void *call_site)__attribute__((no_instrument_function));

void profile_func_enter(void *this_fn, void *call_site) {
    printf("Entering: %p from %p\n", this_fn, call_site);
}

void profile_func_exit(void *this_fn, void *call_site) {
    printf("Exiting: %p from %p\n", this_fn, call_site);
}

int function2() {
    return 0;
}

int function1() {
    sleep(5);
    return function2();
}

int main(int argc, char *argv[]) {
    function1();
    return 0;
}


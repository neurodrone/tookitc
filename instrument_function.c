/**
 * @author Vaibhav Bhembre
 * @version 2013/10/16
 * gcc instrument_function.c -o instrument_function -finstrument-functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <time.h>

void profile_func_enter(void *this_fn, void *call_site)__attribute__((no_instrument_function));
void profile_func_exit(void *this_fn, void *call_site)__attribute__((no_instrument_function));

void profile_func_enter(void *this_fn, void *call_site) {
    Dl_info dl_info;

    this_fn = __builtin_return_address(0);
    if (dladdr(this_fn, &dl_info) == 0) {
        return;
    }

    if (dl_info.dli_sname != NULL
            && dl_info.dli_sname[0] == '_'
            && dl_info.dli_sname[1] == '_') {
        return; /* Only consider in-app functions */
    }

    if (dl_info.dli_sname != NULL
            && dl_info.dli_sname[0] != 0) {
        fprintf(stdout, "Executing %s()\n", dl_info.dli_sname);
    } else {
        fprintf(stdout, "<%p>()\n", this_fn);
    }
}

void profile_func_exit(void *this_fn, void *call_site) {
    printf("Exiting: %p from %p\n", this_fn, call_site);
}

int function2() {
    return 0;
}

int function1() {
    sleep(1);
    return function2();
}

int main(int argc, char *argv[]) {
    function1();
    return 0;
}


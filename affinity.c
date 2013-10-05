/**
 * @author Vaibhav Bhembre
 * @version 2013/10/05
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

int main(int argc, char *argv[]) {
    cpu_set_t my_set, cur_set;
    CPU_ZERO(&my_set);
    CPU_SET(6, &my_set);

    if (sched_getaffinity(getpid(), sizeof (cur_set), &cur_set) < 0) {
        perror("sched_getaffinity");
        return 1;
    }

    printf("pid: '%d' has affinity %08lx\n", getpid(), cur_set);

    if (sched_setaffinity(0, sizeof (my_set), &my_set) < 0) {
        perror("sched_setaffinity");
        return 1;
    }

    if (sched_getaffinity(getpid(), sizeof (cur_set), &cur_set) < 0) {
        perror("sched_getaffinity");
        return 1;
    }

    printf("pid: '%d' has affinity %08lx\n", getpid(), cur_set);
    return 0;
}

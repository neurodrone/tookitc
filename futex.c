/**
 * @author Vaibhav Bhembre
 * @version 2013/10/07
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include "cas.h"
#include "faa.h"

struct fu_q {
    int remaining;
    int consumers;
};

void fu_dec(struct fu_q *q) {
    while (true) {
        int r = q->remaining;
        if (r > 0 &&
            _cas(&q->remaining, r, r - 1)) {
                return;
        }

        _faa(&q->consumers, 1);
        syscall(__NR_futex, &q->remaining, FUTEX_WAIT, r, NULL, 0, 0);
        _faa(&q->consumers, -1);
    }
}

void fu_inc(struct fu_q *q) {
    int old_val = _faa(&q->remaining, 1);
    if (q->consumers > 0) {
        syscall(__NR_futex, &q->remaining, FUTEX_WAKE, old_val, NULL, 0, 0);
    }
}

int main(int argc, char *argv[]) {
    int fd = open("lockfile", O_RDWR);
    assert(fd >= 0);

    struct fu_q *q = mmap(NULL, sizeof (*q),
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED,
                          fd, 0);
    assert(q != MAP_FAILED);

    int pid = fork();
    if (pid == 0) {
        fprintf(stdout, "Sequence start.\n");
        sleep(5);
        fprintf(stdout, "Sequence termination.\n");

        fu_inc(q);
    } else {
        fprintf(stdout, "Awaiting sequence term.\n");
        fu_dec(q);
        fprintf(stdout, "Complete.\n");
    }
    return 0;
}

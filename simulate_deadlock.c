/**
 * @author Vaibhav Bhembre
 * @version 2013/12/15
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

pthread_mutex_t lock1, lock2;

void *print_forever12(void *args) {
    while (1) {
        pthread_mutex_lock(&lock1);
        fprintf(stdout, "Locked lock1...");
        fprintf(stdout, "processing data...");

        pthread_mutex_lock(&lock2);
        fprintf(stdout, "Locked lock2...");
        fprintf(stdout, "processing data...");

        pthread_mutex_unlock(&lock2);
        fprintf(stdout, "Unlocked lock2...\n");
        pthread_mutex_unlock(&lock1);
        fprintf(stdout, "Unlocked lock1...\n");
    }
}

void *print_forever21(void *args) {
    while (1) {
        pthread_mutex_lock(&lock2);
        fprintf(stdout, "Locked lock2...");
        fprintf(stdout, "processing data...");

        pthread_mutex_lock(&lock1);
        fprintf(stdout, "Locked lock1...");
        fprintf(stdout, "processing data...");

        pthread_mutex_unlock(&lock1);
        fprintf(stdout, "Unlocked lock1...\n");
        pthread_mutex_unlock(&lock2);
        fprintf(stdout, "Unlocked lock2...\n");
    }
}

int main(int argc, char *argv[]) {
    pthread_t t1, t2;
    if (pthread_mutex_init(&lock1, NULL) != 0 || pthread_mutex_init(&lock2, NULL) != 0) {
        fprintf(stderr, "Mutex init failed. Error: %s\n", strerror(errno));
        return 1;
    }

    if (pthread_create(&t1, NULL, print_forever12, NULL) != 0) {
        fprintf(stderr, "Pthread creation failed for t1. Error: %s\n", strerror(errno));
        return 1;
    }

    if (pthread_create(&t2, NULL, print_forever21, NULL) != 0) {
        fprintf(stderr, "Pthread creation failed for t2. Error: %s\n", strerror(errno));
        return 1;
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);

    return 0;
}


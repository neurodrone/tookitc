/**
 * @author Vaibhav Bhembre
 * @version 2013/10/03
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_attr_t pat;

int main(int argc, char *argv[]) {
    size_t size;
    char *address;

    pthread_attr_init(&pat);

    if (pthread_attr_getstacksize(&pat, &size) < 0) {
        perror("pthread_attr_getstacksize");
        return 1;
    }

    printf("Stack size: %zu bytes\n", size);

    if (pthread_attr_getstackaddr(&pat, ((void **)&address)) < 0) {
        perror("pthread_attr_getstackaddr");
        return 1;
    }

    printf("Stack addr: %p\n", address);

    pthread_attr_destroy(&pat);

    return 0;
}


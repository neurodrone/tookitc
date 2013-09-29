/**
 * @author Vaibhav Bhembre
 * @version 2013/09/29
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    char *file = "somefile", data[10];

    int fd = open(file, O_WRONLY | O_CREAT | O_EXCL, 00600);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    if (flock(fd, LOCK_EX) < 0) {
        fprintf(stderr, "Unable to lock file: %s\n", file);
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return 1;
    }

    strcpy(data, "somedata\n");

    if (write(fd, data, strlen(data) + 1) < 0) {
        perror("fwrite");
        return 1;
    }

    fprintf(stdout, "Data '%s' written successfully to file: '%s'\n", data, file);

    if (flock(fd, LOCK_UN) < 0) {
        fprintf(stderr, "Unable to unlock file: %s\n", file);
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return 2;
    }

    close(fd);
    unlink(file);

    return 0;
}


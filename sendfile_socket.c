/**
 * @author Vaibhav Bhembre
 * @version 2013/09/10
 * gcc -Wall -Werror -pedantic sendfile_socket.c -o sendfile_socket
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>

#define DEFAULT_PORT "8880"

int main(int argc, char *argv[]) {
    int r, sockfd, fd, f;
    struct addrinfo hint, *res, *p;
    char *port, file[20];
    struct stat statbuf;
    off_t off;
    
    if (argc == 2) {
        if (atoi(argv[1]) > 0) {
            port = argv[1];
        } else {
            port = strdup(DEFAULT_PORT);
        }
    }

    memset(&hint, 0, sizeof (hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;

    if ((r = getaddrinfo(NULL, port, &hint, &res)) != 0) {
        perror("getaddrinfo");
        return 1;
    }

    for (p = res; p; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            perror("socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
            perror("bind");
            continue;
        }
        break;
    }

    assert(p != NULL);

    r = listen(sockfd, 3);
    assert(r >= 0);

    while (1) {
        fd = accept(sockfd, p->ai_addr, &p->ai_addrlen);
        assert(fd >= 0);

        r = recv(fd, file, sizeof (file), 0);
        assert(r >= 0);

        file[r] = 0;
        if (file[strlen(file) - 1] == '\n') {
            file[strlen(file) - 1] = 0;
        }

        if (file[strlen(file) - 1] == '\r') {
            file[strlen(file) - 1] = 0;
        }

        fprintf(stdout, "Sending file: %s\n", file);

        f = open(file, O_RDONLY);
        assert(f >= 0);

        fstat(f, &statbuf);
        
        off = 0;
        r = sendfile(f, fd, off, &statbuf.st_size, NULL, 0);
        assert(r >= 0);

        assert(r == statbuf.st_size);

        close(f);
        close(fd);

    }

    close(sockfd);
    free(port);

    return 0;
}


/**
 * @author Vaibhav Bhembre
 * @version 2013/09/07
 * gcc -Werror -Wall -pedantic distributed_incrementor.c -o distributed_incrementor -lzookeeper_mt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <zookeeper/zookeeper.h>

#define APPID "bhembre.zookeeper_test"
#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT "3181"

long counter;

typedef void (*watcher_f)(zhandle_t *zh,
             int type,
             int state,
             const char *path,
             void *watcherCtx);

zhandle_t *zh;
watcher_f watcher;

zhandle_t *my_zoo_init(char *hostport, watcher_f watcher);
int my_zoo_create(const char *node, const char *val, char *buffer, int buflen);
struct Stat *my_zoo_get_nowatch(const char *node, char *buffer, int *buflen);
char *my_strdup(const char *str);
void noexit_debug(const char *msg);
void exit_debug(const char *msg);
long get_count_from_val(const char *nodename, const char *val);

int main(int argc, char *argv[]) {
    char buf[512], *hostport = NULL;
    char someval[10], somenode[10];
    int rc, buflen;
    struct Stat *stat;

    zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);

    if (argc == 3) {
        hostport = malloc(strlen(argv[1]) + strlen(argv[2]) + 2);
        if (hostport == NULL) {
            exit_debug("malloc failed on `host` or `port` assignment.");
        }

        sprintf(hostport, "%s:%s", argv[1], argv[2]);
    } else if (argc == 2) {
        hostport = my_strdup(argv[1]);
        if (hostport == NULL) {
            exit_debug("malloc failed on `hostport` assignment.");
        }
    }

    zh = my_zoo_init(hostport, watcher);
    free(hostport);
    if (!zh) {
        return 1;
    }

    strcpy(somenode, "/xyz");
    strcpy(someval, "someval");

    rc = my_zoo_create(somenode, someval, buf, sizeof (buf) - 1);
    if (rc != ZOK) {
        noexit_debug("my_zoo_create failed.");
    }

    stat = my_zoo_get_nowatch(somenode, buf, &buflen);
    (void) stat;
    (void) buflen;

    counter = get_count_from_val(somenode, buf);
    if (counter > 0) {
        fprintf(stdout, "Current count: %ld\n", counter);
    } else {
        fprintf(stderr, "Counter failed.\n");
    }

    zookeeper_close(zh);
    return 0;
}

zhandle_t *
my_zoo_init(char *hostport, watcher_f watcher) {
    zhandle_t *zh;
    if (hostport == NULL) {
        char *host = my_strdup(DEFAULT_HOST);
        char *port = my_strdup(DEFAULT_PORT);
        hostport = malloc(strlen(host) + strlen(port) + 2);
        if (!hostport) {
            goto zinit_failure;
        }
        sprintf(hostport, "%s:%s", host, port);
        free(host); free(port);
    }
    fprintf(stdout, "Connecting to '%s'\n", hostport);
    zh = zookeeper_init(hostport,
                        watcher,
                        10 * 1000,
                        0, 0, 0);
    if (!zh) {
        goto zinit_failure;
    }

    return zh;

zinit_failure:
    noexit_debug("zookeeper_init failed.\n");
    return NULL;

}

int
my_zoo_create(const char *node,
              const char *val,
              char *buffer,
              int buflen) {
    int vallen = strlen(val);
    return zoo_create(zh, node, val, vallen, &ZOO_OPEN_ACL_UNSAFE, ZOO_SEQUENCE | ZOO_EPHEMERAL, buffer, buflen);
}

struct Stat *
my_zoo_get_nowatch(const char *node,
                   char *buffer,
                   int *buflen) {
    struct Stat *stat = malloc(sizeof (*stat));
    int rc = zoo_get(zh, node, 0, buffer, buflen, stat);
    if (!rc || !stat) {
        return NULL;
    }
    return stat;
}

          

char *
my_strdup(const char *str) {
    char *nstr = malloc(strlen(str) + 1);
    if (nstr == NULL) {
        return NULL;
    }
    memcpy(nstr, str, strlen(str) + 1);
    return nstr;
}

void noexit_debug(const char *msg) {
    fprintf(stderr, "Error[%d]: %s\n", errno, msg);
}

void exit_debug(const char *msg) {
    fprintf(stderr, "Error[%d]: %s\n", errno, msg);
    exit(errno);
}

long
get_count_from_val(const char *nodename, const char *val) {
    if (!nodename || !val
                  || strstr(val, nodename) == NULL) {
        return -1;
    }

    while (*nodename++ == *val++); val--;

    return strtol(val, NULL, 10);
}

/**
 * @author Vaibhav Bhembre
 * @version 2013/09/07
 * gcc -Werror -Wall -pedantic distributed_incdec.c -o distributed_incdec -lzookeeper_mt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <zookeeper/zookeeper.h>

#define APPID "bhembre.zookeeper_test"
#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT "3181"

long inc_ctr, dec_ctr;

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
long get_count_from_val(struct Stat *stat);
const char *get_parent(const char *node);

int main(int argc, char *argv[]) {
    const char *parent;
    char buf[512], *hostport = NULL;
    char incval[2], incnode[10];
    char decval[2], decnode[10];
    int rc = 0, buflen, incdec = 0;
    long counter;
    struct Stat *stat;

    zoo_set_debug_level(ZOO_LOG_LEVEL_INFO);

    if (argc == 2) {
        incdec = atoi(argv[1]);
    }

    zh = my_zoo_init(hostport, watcher);
    free(hostport);
    if (!zh) {
        return 1;
    }

    strcpy(incnode, "/inc/inc");
    strcpy(incval, "");

    strcpy(decnode, "/dec/dec");
    strcpy(decval, "");

    if (incdec >= 0) {
        rc |= my_zoo_create(incnode, incval, buf, sizeof (buf) - 1);
    }
    if (incdec <= 0) {
        rc |= my_zoo_create(decnode, decval, buf, sizeof (buf) - 1);
    }

    if (rc != ZOK) {
        noexit_debug("my_zoo_create failed.\n");
    }

    parent = get_parent(incnode);
    stat = my_zoo_get_nowatch(parent, buf, &buflen);
    inc_ctr = (get_count_from_val(stat) - 1) / 2; /* creation and deletion are two separate changes */
    if (inc_ctr < 0) {
        exit_debug("Increment counter failed.\n");
    }
    free((char *)parent);
    free(stat);

    parent = get_parent(decnode);
    stat = my_zoo_get_nowatch(parent, buf, &buflen);
    dec_ctr = (get_count_from_val(stat) - 1) / 2;
    if (dec_ctr < 0) {
        exit_debug("Decrement counter failed.\n");
    }
    free((char *)parent);
    free(stat);

    (void) stat;
    (void) buflen;

    counter = inc_ctr - dec_ctr;
    fprintf(stdout, "Current count: %ld\n", counter);

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
    if (rc != ZOK || !stat) {
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
get_count_from_val(struct Stat *stat) {
    if (!stat) {
        return -1;
    }
    return stat->cversion;
}

const char *
get_parent(const char *node) {
    const char *endptr;
    char *str;
    ptrdiff_t diff;

    if (strchr(node, '/') == strrchr(node, '/')) { /* account if the node is the root node */
        return node;
    }

    endptr = node;
    endptr = strchr(++endptr, '/');
    diff = endptr - node;

    str = calloc(1, diff + 1);
    if (!str) {
        exit_debug("calloc failed for get_parent.");
    }

    memcpy(str, node, diff);
    return str;
}

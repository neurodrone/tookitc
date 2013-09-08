/**
 * @author Vaibhav Bhembre
 * @version 2013/09/07
 * @description A distributed highly-consistent counter using ZK.
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

char incnode[10], decnode[10];

typedef void (*watcher_f)(zhandle_t *zh,
             int type,
             int state,
             const char *path,
             void *watcherCtx);

zhandle_t *zh;
watcher_f watcher;

void initialize_zookeeper();
void increment_counter();
void decrement_counter();
long get_increment();
long get_decrement();
long get_count();
long get_node_count();
zhandle_t *my_zoo_init(char *hostport, watcher_f watcher);
int my_zoo_create(const char *node, const char *val, char *buffer, int buflen);
int my_zoo_createi(const char *node, const char *val, int vallen, char *buffer, int buflen, int flags);
void my_zoo_get_nowatch(const char *node, char *buffer, int *buflen, struct Stat *stat);
char *my_strdup(const char *str);
void noexit_debug(const char *msg);
void exit_debug(const char *msg);
long get_count_from_val(struct Stat *stat);
const char *get_parent(const char *node);

int main(int argc, char *argv[]) {
    long counter;
    long incdec = 0;

    if (argc == 2) {
        incdec = atoi(argv[1]);
    }

    strcpy(incnode, "/inc/inc");
    strcpy(decnode, "/dec/dec");

    initialize_zookeeper();

    if (incdec > 0) {
        increment_counter();
    }

    if (incdec < 0) {
        decrement_counter();
    }

    counter = get_count();
    fprintf(stdout, "Current count: %ld\n", counter);

    exit_debug("");
    return 0;
}

void
increment_counter() {
    char buf[512];
    int rc;
    rc = my_zoo_create(incnode, "1", buf, sizeof (buf) - 1);
    if (rc != ZOK) {
        noexit_debug("Failed to increment counter.");
    }
}

void
decrement_counter() {
    char buf[512];
    int rc;
    rc = my_zoo_create(decnode, "1", buf, sizeof (buf) - 1);
    if (rc != ZOK) {
        noexit_debug("Failed to decrement counter.");
    }
}

long
get_increment() {
    const char *parent;
    long inc_ctr;

    parent = get_parent(incnode);
    inc_ctr = get_node_count(parent);
    if (inc_ctr < 0) {
        exit_debug("Increment counter failed.");
    }
    inc_ctr = (inc_ctr - 1) / 2; /* creation and deletion are two separate changes */
    free((char *)parent);

    return inc_ctr;
}

long
get_decrement() {
    const char *parent;
    long dec_ctr;

    parent = get_parent(decnode);
    dec_ctr = get_node_count(parent);
    if (dec_ctr < 0) {
        exit_debug("Decrement counter failed.");
    }
    dec_ctr = (dec_ctr - 1) / 2; /* creation and deletion are two separate changes */
    free((char *)parent);

    return dec_ctr;
}

long
get_node_count(const char *node) {
    int ctr, buflen = 2;
    struct Stat stat;
    char buf[buflen];

    my_zoo_get_nowatch(node, buf, &buflen, &stat);
    ctr = get_count_from_val(&stat);

    return ctr;
}

long
get_count() {
    long inc, dec;
    inc = get_increment();
    dec = get_decrement();

    return inc - dec;
}

int
znode_exists(const char *node) {
    int rc;
    struct Stat *stat = malloc(sizeof (*stat));
    rc = zoo_exists(zh, node, 0, stat);

    free(stat);
    return rc != ZNONODE;
}

void
initialize_zookeeper() {
    const char *parent;
    char buf[512];
    int rc;
    zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);

    zh = my_zoo_init(NULL, watcher); /* extend this to connect to non-default servers */
    if (!zh) {
        exit_debug("Unable to initialize zookeeper.");
    }

    parent = get_parent(incnode);
    if (!znode_exists(parent)) {
        rc = my_zoo_createi(parent, "1", 1, buf, sizeof (buf) - 1, 0);
        if (rc != ZOK) {
            noexit_debug("znode exists failed for incnode parent.");
        }
    }
    free((char *)parent);

    parent = get_parent(decnode);
    if (!znode_exists(parent)) {
        rc = my_zoo_createi(parent, "1", 1, buf, sizeof (buf) - 1, 0);
        if (rc != ZOK) {
            noexit_debug("znode exists failed for decnode parent.");
        }
    }
    free((char *)parent);
}

zhandle_t *
my_zoo_init(char *hostport, watcher_f watcher) {
    zhandle_t *zh;
    char *hp;
    if (hostport == NULL) {
        char *host = my_strdup(DEFAULT_HOST);
        char *port = my_strdup(DEFAULT_PORT);
        hp = malloc(strlen(host) + strlen(port) + 2);
        if (!hp) {
            goto zinit_failure;
        }
        sprintf(hp, "%s:%s", host, port);
        free(host); free(port);
    } else {
        hp = my_strdup(hostport);
    }
    fprintf(stdout, "Connecting to '%s'\n", hp);
    zh = zookeeper_init(hp,
                        watcher,
                        10 * 1000,
                        0, 0, 0);
    free(hp);
    if (!zh) {
        goto zinit_failure;
    }

    return zh;

zinit_failure:
    noexit_debug("zookeeper_init failed.");
    return zh;
}

int
my_zoo_create(const char *node,
              const char *val,
              char *buffer,
              int buflen) {
    int vallen = strlen(val);
    return my_zoo_createi(node, val, vallen, buffer, buflen, ZOO_SEQUENCE | ZOO_EPHEMERAL);
}

int
my_zoo_createi(const char *node,
               const char *val,
               int vallen,
               char *buffer,
               int buflen,
               int flags) {
    return zoo_create(zh, node, val, vallen, &ZOO_OPEN_ACL_UNSAFE, flags, buffer, buflen);
}

void
my_zoo_get_nowatch(const char *node,
                   char *buffer,
                   int *buflen,
                   struct Stat *stat) {
    int rc;
    rc = zoo_get(zh, node, 0, buffer, buflen, stat);
    if (rc != ZOK) {
        noexit_debug("zoo_get did not return ZOK, please examine.");
    }
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
    fprintf(stderr, "%s:%d: Error[%d]: %s\n", __FILE__, __LINE__, errno, msg);
}

void exit_debug(const char *msg) {
    if (strlen(msg)) {
        fprintf(stderr, "%s:%d: Error[%d]: %s\n", __FILE__, __LINE__, errno, msg);
    }
    zookeeper_close(zh);
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
    const char *endptr, *tmpptr;
    char *str;
    ptrdiff_t diff;

    if (strchr(node, '/') == strrchr(node, '/')) { /* account if the node is the root node */
        return node;
    }

    endptr = node;
    tmpptr = endptr = strchr(++endptr, '/');
    while (tmpptr) {
        endptr = tmpptr;
        tmpptr = strchr(++tmpptr, '/');
    }
    diff = endptr - node;

    str = calloc(1, diff + 1); /* Allocate exact size for parent node name */
    if (!str) {
        exit_debug("calloc failed for get_parent.");
    }

    memcpy(str, node, diff);
    return str;
}

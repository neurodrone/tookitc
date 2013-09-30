/**
 * @author Vaibhav Bhembre
 * @version 2013/09/30
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <microhttpd.h>

#define PAGE "<html><head><title>Libmicrodemo</title></head><body>libmicrohttpd demo</body></html>"

static bool method_is_GET(const char *method) {
    return strcmp(method, MHD_HTTP_METHOD_GET) == 0;
}

static int handler(void *cls,
                   struct MHD_Connection *conn,
                   const char *url,
                   const char *method,
                   const char *version,
                   const char *upload_data,
                   size_t *upload_data_size,
                   void **ptr) {
    static int aptr;
    const char *me = cls;
    struct MHD_Response *response;
    int ret;

    if (!method_is_GET(method)) {
        return MHD_NO;
    }

    if (&aptr != *ptr) {
        *ptr = &aptr;
        return MHD_YES;
    }

    *ptr = NULL;
    response = MHD_create_response_from_buffer(strlen(me),
                                               (void *) me,
                                               MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(conn, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

int main(int argc, char *argv[]) {
    struct MHD_Daemon *d;

    if (argc != 2) {
        fprintf(stderr, "Need port to continue.\n");
        return 1;
    }

    d = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG,
                         atoi(argv[1]),
                         NULL, NULL, &handler, PAGE,
                         MHD_OPTION_CONNECTION_TIMEOUT, 120,
                         MHD_OPTION_END);

    if (d == NULL) {
        return 1;
    }

    getchar();
    MHD_stop_daemon(d);
    return 0;
}


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void display_proc_map();
char *trim(const char *str);
char *selectText(const char *str, char a, char b);

int main(void) {

    int i = 200 * 1000, j;

    fprintf(stdout, "Program initiation...\n");
    display_proc_map();

    int *m = malloc(i * sizeof (*m));
    fprintf(stdout, "\n\nOn malloc call...\n");
    display_proc_map();

    for (j = 0; j < i; j++) {
        m[j] = j;
    }
    fprintf(stdout, "\n\nOn actual allocation...\n");
    display_proc_map();

    return 0;
}

void display_proc_map() {
    FILE *fp = fopen("/proc/self/maps", "r");
    char line[1024];

    long unsigned int start_addr, end_addr, size;

    while (fgets(line, sizeof (line), fp) != NULL) {
        char *start, *end, *p = line;
        int i;

        start = p;
        p = strchr(p, '-');
        if (!p)
            continue;
        *p++ = '\0';

        end = p;
        p = strchr(p, ' ');
        if (!p)
            continue;
        *p++ = '\0';

        if (!strstr(p, "00:00"))
            continue;

        if (!strstr(p, "rw"))
            continue;

        if (strstr(p, "stack") ||
            strstr(p, "vdso") ||
            strstr(p, "vsyscall"))
            continue;

        start_addr = strtoul(start, NULL, 16);
        end_addr = strtoul(end, NULL, 16);
        size = end_addr - start_addr;

        fprintf(stdout, "Testing mem at %lx (%lu bytes) -- %s\n", start_addr, size, selectText(trim(p), '[', ']'));
    }

    fclose(fp);
}

char *trim(const char *str) {
    int len = strlen(str);
    char *trim_str, *temp_str, *ret_str;

    trim_str = malloc(len + 1);
    ret_str = trim_str;
    temp_str = strdup(str);
    while (*temp_str++) {
        if (*temp_str != ' ')
            *trim_str++ = *temp_str;
    }
    *trim_str = '\0';
    return ret_str;

}

char *selectText(const char *str, char start, char end) {
    int len = strlen(str);
    char *trim_str, *ret_str;

    trim_str = malloc(len + 1);
    ret_str = trim_str;

    while (*str && *str++ != start) continue;
    if (!*str) return NULL;

    while (*str && *str != end) *trim_str++ = *str++;
    if (ret_str == trim_str) return NULL;

    *trim_str = '\0';

    return ret_str;
    
}

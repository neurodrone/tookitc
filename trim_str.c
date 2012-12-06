#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *trim(const char *str, char a);

int main(void) {
    char *str = "   EL  EPHANT   ";
    printf("Trimmed output of '%s' is: '%s'\n", str, trim(str, ' '));
    return 0;
}

char *trim(const char *str, char r) {
    int len = strlen(str);
    char *trim_str, *temp_str, *ret_str;

    trim_str = malloc(len + 1);
    ret_str = trim_str;
    temp_str = strdup(str);

    while (*temp_str != '\0') {
        if (*temp_str != r)
            *trim_str++ = *temp_str;
        temp_str++;
    }
    *trim_str = '\0';
    return ret_str;

}

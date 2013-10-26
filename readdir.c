/**
 * @author Vaibhav Bhembre
 * @version 2013/10/25
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>

int main(int argc, char *argv[]) {
    DIR *dirp;
    struct dirent *dp;

    dirp = opendir(".");
    assert(dirp != NULL);

    do {
        dp = readdir(dirp);
        if (dp == NULL) {
            break;
        }

        printf("Name: %s, inode: %" PRIu64 "\n", dp->d_name, dp->d_ino);
    } while (dp != NULL);

    closedir(dirp);

    return 0;
}


/**
 * @author Vaibhav Bhembre
 * @version 2013/10/25
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <assert.h>
#include <inttypes.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    DIR *dirp;
    struct dirent *dp;
    struct stat stat_buf;

    dirp = opendir(".");
    assert(dirp != NULL);

    do {
        dp = readdir(dirp);
        if (dp == NULL) {
            break;
        }

        printf("Name: %s, inode: %" PRIu64, dp->d_name, dp->d_ino);

        stat(dp->d_name, &stat_buf);
        printf(", size: %" PRIu64 " bytes.\n", stat_buf.st_size);
    } while (dp != NULL);

    closedir(dirp);

    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>

#include <api/glfs.h>

#define DEFAULT_PORT 24007
#define DEFAULT_TRANSPORT "tcp"
#define DEFAULT_LOGFILE "/dev/stderr"
#define DEFAULT_LOGLEVEL 3

#define MAXPATH 32 * 1024

glfs_t *fs = NULL;

int init(const char *volname, const char *hostname) {
    fs = glfs_new(volname);
    assert(NULL != fs);

    assert(0 == glfs_set_volfile_server(fs, DEFAULT_TRANSPORT, hostname, DEFAULT_PORT));
    assert(0 == glfs_set_logging(fs, DEFAULT_LOGFILE, DEFAULT_LOGLEVEL));
    assert(0 == glfs_init(fs));
    assert(0 == glfs_setfsuid(0));
    assert(0 == glfs_setfsgid(0));

    return 0;
}

int find(const char *dir, const char *filename) {
    int found = 0, err = 0;
    struct dirent *f;
    struct stat stat;
    char path[MAXPATH + 1] = {0};
    glfs_fd_t *fd;
    if (!dir || dir[0] == '.') return 0;

    if (glfs_stat(fs, dir, &stat)) {
        err = errno;
        fprintf(stderr, "[ ERROR ]: stat(%s): %s\n", dir, strerror(err));
        return 0;
    }
    if (!S_ISDIR(stat.st_mode)) {
        return 0;
    }

    fd = glfs_opendir(fs, dir);
    if (!fd) {
        err = errno;
        fprintf(stderr, "[ ERROR ]: glfs_opendir(%s): %s\n", dir, strerror(err));
        return 0;
    }

    long off = glfs_telldir(fd);

    // first walk through files
    while((f = glfs_readdirplus(fd, &stat)) != NULL) {
        if (!S_ISREG(stat.st_mode)) continue;

        //  printf("[FILE]: %s/%s\n", dir, f->d_name);
        found = (S_ISREG(stat.st_mode) && strcmp(f->d_name, filename) == 0);
        if (found) {
            printf("%s/%s\n", dir, filename);
            goto out;
        }
    }

    // then rewind and walk through subdirectories
    glfs_seekdir(fd, off);
    while((f = glfs_readdirplus(fd, &stat)) != NULL) {
        if (!S_ISDIR(stat.st_mode) || f->d_name[0] == '.') continue;

        bzero(path, sizeof(path));
        snprintf(path, MAXPATH, "%s/%s", dir, f->d_name);
        // printf("[DIR]: %s\n", path);
        found = find(path, filename);
        if (found) {
            goto out;
        }
    }

out:
    if (glfs_closedir(fd)) {
        err = errno;
        fprintf(stderr, "[ ERROR ]: glfs_closedir(%s): %s\n", path, strerror(err));
    }

    return found;
}


int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf ("Expect following args\n\t%s <volname> <hostname> <filename> [root=\"/\"]\n", argv[0]);
        return -1;
    }

    assert(init(argv[1], argv[2]) == 0);
    find((argc > 4) ? argv[4] : "/", argv[3]);
    assert(0 == glfs_fini(fs));

    return 0;
}

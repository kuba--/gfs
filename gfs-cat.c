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

#define BUFSIZE 4096

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

void cat(const char *path) {
    int err = 0;
    size_t n = 0;
    struct stat sb;
    struct dirent *f;
    glfs_fd_t *fd;
    unsigned char buf[BUFSIZE + 1] = {0};

    if (glfs_stat(fs, path, &sb)) {
        err = errno;
        fprintf(stderr, "[ ERROR ]: stat(%s): %s\n", path, strerror(err));
        return;
    }

    if (S_ISDIR(sb.st_mode)) {
        fd = glfs_opendir(fs, path);
        if (!fd) {
            err = errno;
            fprintf(stderr, "[ ERROR ]: glfs_opendir(%s): %s\n", path, strerror(err));
            return;
        }

        while((f = glfs_readdir(fd)) != NULL) {
            printf("%s\n", f->d_name);
        }

        if (glfs_closedir(fd)) {
            err = errno;
            fprintf(stderr, "[ ERROR ]: glfs_closedir(%s): %s\n", path, strerror(err));
        }

    } else if (S_ISREG(sb.st_mode)) {
        fd = glfs_open(fs, path, O_RDONLY);
        if (!fd) {
            err = errno;
            fprintf(stderr, "[ ERROR ]: glfs_open(%s): %s\n", path, strerror(err));
            return;
        }

        while((n = glfs_read(fd, buf, BUFSIZE, 0)) > 0) {
            fwrite(buf, sizeof(buf[0]), n, stdout);
        }

        if (glfs_close(fd)) {
           err = errno;
           fprintf(stderr, "[ ERROR ]: glfs_close(%s): %s\n", path, strerror(err));
        }
    }
}

int main(int argc, char *argv[]) {
    int i = 3;
    if (argc < 4) {
        printf ("Expect following args\n\t%s <volname> <hostname> <path>...\n", argv[0]);
        return -1;
    }

    assert(init(argv[1], argv[2]) == 0);
    for(; i < argc; ++i)
        cat(argv[i]);

    assert(0 == glfs_fini(fs));

    return 0;
}

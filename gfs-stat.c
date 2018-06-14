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

void print_stat(const char *path) {
    int err = 0;
    struct stat sb;

    if (glfs_stat(fs, path, &sb)) {
        err = errno;
        fprintf(stderr, "[ ERROR ]: stat(%s): %s\n", path, strerror(err));
        return;
    }

    printf("Path: %s\n", path);
    printf("File type: ");
    switch (sb.st_mode & S_IFMT) {
        case S_IFBLK:  printf("block device\n");            break;
        case S_IFCHR:  printf("character device\n");        break;
        case S_IFDIR:  printf("directory\n");               break;
        case S_IFIFO:  printf("FIFO/pipe\n");               break;
        case S_IFLNK:  printf("symlink\n");                 break;
        case S_IFREG:  printf("regular file\n");            break;
        case S_IFSOCK: printf("socket\n");                  break;
        default:       printf("unknown?\n");                break;
    }
    printf("I-node number: %ld\n", (long) sb.st_ino);
    printf("Mode: %lo (octal)\n", (unsigned long) sb.st_mode);
    printf("Link count: %ld\n", (long) sb.st_nlink);
    printf("Ownership: UID=%ld GID=%ld\n", (long) sb.st_uid, (long) sb.st_gid);
    printf("Preferred I/O block size: %ld bytes\n", (long) sb.st_blksize);
    printf("File size: %lld bytes\n", (long long) sb.st_size);
    printf("Blocks allocated: %lld\n", (long long) sb.st_blocks);
    printf("Last status change: %s", ctime(&sb.st_ctime));
    printf("Last file access: %s", ctime(&sb.st_atime));
    printf("Last file modification: %s", ctime(&sb.st_mtime));
    printf("\n");
}

int main(int argc, char *argv[]) {
    int i = 3;
    if (argc < 4) {
        printf ("Expect following args\n\t%s <volname> <hostname> <path>...\n", argv[0]);
        return -1;
    }

    assert(init(argv[1], argv[2]) == 0);
    for(; i < argc; ++i)
        print_stat(argv[i]);

    assert(0 == glfs_fini(fs));

    return 0;
}

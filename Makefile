CC = gcc
CFLAGS = -Wall -O2
INCLUDES = -I/usr/include/glusterfs
LFLAGS = -L/usr/lib64
LIBS = -lgfapi

.PHONY:	clean

all:	gfs-stat	gfs-find	gfs-cat

gfs-stat	gfs-find	gfs-cat:
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $@.c $(LFLAGS) $(LIBS)

clean:
	$(RM) *.o *~ gfs-stat	gfs-find	gfs-cat

# gfs
GlusterFS API utils.

All scripts use GlusterFS' native library to _stat_, _find_, _cat_ files (doesn't matter if you use FUSE, NFS or any other protocol).

### building

```
$ sudo yum update
# install dependencies (glusterfs-api)
$ sudo yum install gcc make glusterfs-devel glusterfs-api-devel
# compile all
$ sudo make all
```

### use cases

* gfs-stat - prints information about Gluster's i-node.
```
$ ./gfs-stat volume1 10.10.10.10 /root

Path: /root
File type: directory
I-node number: -5255060368930443126
Mode: 40755 (octal)
Link count: 258
Ownership: UID=0 GID=0
Preferred I/O block size: 4096 bytes
File size: 4096 bytes
Blocks allocated: 8
Last status change: Wed Jun 13 13:53:12 2018
Last file access: Wed Jun 13 13:53:13 2018
Last file modification: Fri May 18 14:13:47 2018
```

* gfs-find - finds a filename and prints absolute path on Gluster's brick.
```
$  ./gfs-find volume1 10.10.10.10 009f1b44edbf3a7f6d55a73c5b5142184aa4d391.siva

/repositories/00/009f1b44edbf3a7f6d55a73c5b5142184aa4d391.siva
```

* gfs-cat - prints a file's content (for directories it lists a directory).
```
$  ./gfs-cat volume1 10.10.10.10 /repositories/00

# ...
00215d96da934c254a52409f6e813b694e723cdf.siva
00bf43b5fd9e75f13b9f74f1d60f77c1fae1c4fa.siva
007cb81cdbd4efca266f1cad5fc5410e1d8352e3.siva
00ea22711a73322f36733e20644e272e77a163d2.siva
# ...
```

### piping utils together

* print stats for all files in directory
```
$ ./gfs-cat volume1 10.10.10.10 /repositories/00 | xargs -I {} ./gfs-stat volume1 10.10.10.10 {} /repositories/00/{}

# ...
Path: /repositories/00/00a43ec9c10c4ed4f4ae2b5ea0076e14335845bf.siva
File type: regular file
I-node number: -7425797967187790928
Mode: 100644 (octal)
Link count: 1
Ownership: UID=0 GID=0
Preferred I/O block size: 4096 bytes
File size: 10568 bytes
Blocks allocated: 21
Last status change: Fri May 18 19:11:51 2018
Last file access: Fri May 18 19:11:51 2018
Last file modification: Fri May 18 19:11:51 2018

Path: /repositories/00/00c608a081eb704b6a73fb7afb8672d1f91648b8.siva
File type: regular file
I-node number: -6895058248858604066
Mode: 100644 (octal)
Link count: 1
Ownership: UID=0 GID=0
Preferred I/O block size: 4096 bytes
File size: 112340 bytes
Blocks allocated: 220
Last status change: Fri May 18 14:14:26 2018
Last file access: Fri May 18 14:14:26 2018
Last file modification: Fri May 18 14:14:26 2018
# ...
```

* copy file
```
$ ./gfs-cat volume1 10.10.10.10 /repositories/00/00f6d0cd71e4762aec9ec94d2489d96e464734ee.siva > /tmp/00/00f6d0cd71e4762aec9ec94d2489d96e464734ee.siva.bak
```

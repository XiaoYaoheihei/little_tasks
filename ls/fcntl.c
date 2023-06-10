int flags;
flags = fcntl(fd, F_GETFL);
if (flags == -1) {
    printf("kaobeifubenshibai");
} else {
    flags |= O_APPEND;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        printf("fail to update");
    }
}
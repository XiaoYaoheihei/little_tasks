#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/time.h>
#include<fcntl.h>
#include<sys/mman.h>

int get_size(char *str, char ch) {
    int size = 0;
    int length = strlen(str);
    for (int i =0; i < length; i++) {
        size+=((int)str[i]-48)*(length-i);
    }

    switch (ch)
    {
    case 'k':
    case 'K':
        return (int)(size/sizeof(int));
        break;
    case 'm':
    case 'M':
        return (int)(1024*size/sizeof(int));
        break;
    case 'G':
    case 'g':
        return (int)(1024*1024*size/sizeof(int));
        break;
    default:
        exit(2);
    }
}

void* read_write(void *s) {
    int size = (int)s;

    int *array = (int*)malloc(sizeof(int)*(size));
    
    struct timeval t1,t2;
    //记录执行之前的时间
    gettimeofday(&t1, NULL);
    int fd = open("mmap_test", O_RDWR);
    off_t offset = lseek(fd, 0, SEEK_SET);
    if (offset == -1) {
        perror("lseek error");
        exit(1);
    }
    if (sizeof(int)*(size) != read(fd, (void*)array, sizeof(int)* (size))) {
        printf("read error\n");
    }
    // if (sizeof(int)*(size) != write(fd, (void*)array, sizeof(int)* (size))) {
    //     printf("write error");
    // }

    close(fd);
    //执行之后的函数
    gettimeofday(&t2, NULL);
    printf("time of read_write:%ldms\n", t2.tv_usec-t1.tv_usec);
    return NULL;
}

void* map(void *s) {
    int size = (int)s;
    // int size = *tmp;
    int *array = (int*)malloc(sizeof(int)*(size));
    
    struct timeval t1,t2;
    gettimeofday(&t1, NULL);
    int fd = open("mmap_test", O_RDWR);
    off_t offset = lseek(fd, 0, SEEK_SET);
    if (offset == -1) {
        perror("lseek error");
        exit(1);
    }
    array = mmap(NULL, sizeof(int)*(size), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    
    munmap(array, sizeof(int)*(size));
    msync(array, sizeof(int)*(size), MS_SYNC);

    close(fd);
    gettimeofday(&t2, NULL);
    printf("time of mmap:%ldms\n", t2.tv_usec-t1.tv_usec);
    return NULL;
}

int main(int argc, char *argv[]) {
    FILE *fp;
    pthread_t t1,t2;
    if (argc != 3) {
        perror("error");
        exit(1);
    }
    //读取设置大小内存
    int size = get_size(argv[1],(char)argv[2][0]);
    fp=fopen("mmap_test","w");
    if (!fp) {
        perror("error");
    }
    for (int i = 0; i < size; i++) {
        fprintf(fp,"%d",i);
    }
    fprintf(fp,"\n");
    fclose(fp);
    //执行read_write函数
    pthread_create(&t1,NULL,read_write,(void *)(__intptr_t)size);
    //执行mmap函数执行
    pthread_create(&t2,NULL,map,(void *)(__intptr_t)size);
    //阻塞线程完成函数
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    return 0;
}


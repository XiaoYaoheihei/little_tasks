#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#define PTHREAD_NUM 16
unsigned long sum = 0;
void thread() {
    for (int i = 0; i < 10000; i++) {
        //没有使用原子锁
        sum += 1;
    }
}
int main() {
    printf("before num = %lu\n", sum);
    //创建的线程标识
    pthread_t pthread[PTHREAD_NUM];
    int ret;
    void *retval[PTHREAD_NUM];

    for (int i = 0 ; i < PTHREAD_NUM; i++) {
        ret = pthread_create(&pthread[i], NULL, thread, NULL);
        if (ret != 0) {
            perror("cause:");
            printf("creat pthread %d failed \n", i+1);
        }
    }

    for (int i = 0; i < PTHREAD_NUM; i++) {
        pthread_join(pthread[i], &retval[i]);
    }
    printf("after num = %lu\n", sum);
    return 0;
}

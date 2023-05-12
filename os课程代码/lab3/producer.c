#include "test.h"
int buff[M] = {0};     /*缓冲初始化为0， 开始时没有产品*/
int in = 0;              /*生产者放置产品的位置*/
int out = 0;             /*消费者取产品的位置*/

sem_t empty_sem;         /*同步信号量，当满了时阻止生产者放产品*/
sem_t full_sem;          /*同步信号量，当没产品时阻止消费者消费*/
pthread_mutex_t mutex;   /*互斥信号量， 一次只有一个线程访问缓冲*/
void print()
{
    int i;
    for(i = 0; i < M; i++)
        printf("%d ", buff[i]);
    printf("\n");
}

void producer()
{
    for(;;)
    {
        sleep(2);

        P(empty_sem);
        pthread_mutex_lock(&mutex);

        in = in % M;
        printf("(+)produce a product. buffer:");

        buff[in] = 1;
        // print(buff);
        for(int i = 0; i < M; i++)
          printf("%d ", buff[i]);
        printf("\n");
        ++in;

        pthread_mutex_unlock(&mutex);
        V(full_sem);
    }
}

void consumer()
{
    for(;;)
    {
        sleep(1);

        P(full_sem);
        // printf("sdkljfhasfjashdfjasfh\n");
        pthread_mutex_lock(&mutex);

        out = out % M;
        printf("(-)consume a product. buffer:");

        buff[out] = 0;
        // print(buff);
        for(int i = 0; i < M; i++)
          printf("%d ", buff[i]);
        printf("\n");
        ++out;

        pthread_mutex_unlock(&mutex);
        V(empty_sem);
    }
}
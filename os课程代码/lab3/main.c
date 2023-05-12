//pv操作：生产者与消费者经典问题
#include "test.h"
void *pro()
{
  producer();
  // producer();
}

void *con()
{
  // consumer();
  consumer();
}

void sem_mutex_init()
{
    int init1 = sem_init(&empty_sem, 0, M);
    int init2 = sem_init(&full_sem, 0, 0);
    if( (init1 != 0) && (init2 != 0))
    {
        printf("sem init failed \n");
        exit(1);
    }

    int init3 = pthread_mutex_init(&mutex, NULL);
    if(init3 != 0)
    {
        printf("mutex init failed \n");
        exit(1);
    }
    
}
int main()
{
    pthread_t id1[5];
    pthread_t id2[4];
    int i;
    int ret;

    sem_mutex_init();
    for (int i = 0; i < 5; i++) {
        ret = pthread_create(&id1[i], NULL, pro, NULL);
        if(ret != 0)
        {
            printf("producer creation failed \n");
            exit(1);
        }
    }
    
    for (int i=0;i < 4;i++) {
        ret = pthread_create(&id2[i], NULL, con, NULL);
        if(ret != 0)
        {
            printf("consumer creation failed \n");
            exit(1);
        }
    }
   
    for (int i= 0;i<5;i++) {
        pthread_join(id1[i],NULL);
    }
    for(int i= 0;i < 4;i++) {
        pthread_join(id2[i],NULL);
    }
    

    exit(0);
}

#ifndef TEST_H_
#define TEST_H_
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define M 32             /*缓冲数目*/
#define P(x)     sem_wait(&x)
#define V(x)    sem_post(&x)

extern sem_t empty_sem;
extern sem_t full_sem;
extern pthread_mutex_t mutex;

void consumer();
void producer();
// void consumer(sem_t, sem_t, pthread_mutex_t, int, int*);
// void producer(sem_t, sem_t, pthread_mutex_t, int, int*);
void print();


#endif
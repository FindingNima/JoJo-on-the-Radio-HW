#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define TRUE 1
#define FALSE 0
#define N 100
#define MAX_THREADS 2

void producer(void);
void consumer(void);
int produce_item();
void insert_item(int);
int remove_item();
void consume_item(int);

sem_t mutex;
sem_t empty;
sem_t full;

int count = 0;
int items[N];

void producer(void)
{
    int item;

    while(TRUE)
    {
        item = produce_item();
        sem_wait(&empty);
        sem_wait(&mutex);
        insert_item(item);
        sem_post(&mutex);
        sem_post(&full);
    }
    pthread_exit(NULL);
}

void consumer(void)
{
    int item;

    while(TRUE)
    {
        item = remove_item();
        sem_wait(&full);
        sem_wait(&mutex);
        consume_item(item);
        sem_post(&mutex);
        sem_post(&empty);
    }
    pthread_exit(NULL);
}

int produce_item()
{
    int item = rand();
    printf("produced %d\n", item);
    return item;
}

void insert_item(int item)
{
    items[count++] = item;
}

int remove_item()
{
    return items[--count];
}

void consume_item(int item)
{
    printf("consumed %d\n", item);
    return;
}

int main()
{
    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, N);

    pthread_t threads[MAX_THREADS];
    int rc;
    long t;
    t = 1;
    rc = pthread_create(&threads[0], NULL, (void *) &producer, (void *) t);
    t = 2;
    rc = pthread_create(&threads[1], NULL, (void *) &consumer, (void *) t);

    pthread_exit(NULL);
}
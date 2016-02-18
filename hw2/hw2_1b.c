#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

void producer(void *t);
void consumer(void *t);
void print_buffer_status();

int max_items = 0;
int current_items = 0;
int* buffer_matrix;
int shared_buffers;

pthread_mutex_t prod_count_mutex;
pthread_mutex_t buffer_mutex;
pthread_mutex_t* index_mutexs;


int producer_meets_conditions()
{
        pthread_mutex_lock(&prod_count_mutex);
        if (current_items >= max_items)
        {
            pthread_mutex_unlock(&prod_count_mutex);
            return 0;
        }
        current_items++;
        printf("%d items out of %d\n", current_items, max_items);
        pthread_mutex_unlock(&prod_count_mutex);
        return 1;
}

void producer(void *t)
{
    int i;
    while (producer_meets_conditions())
    {
        pthread_mutex_lock(&buffer_mutex);
        int foundOne = FALSE;
        for (i = 0; i < shared_buffers; i++)
        {
            pthread_mutex_lock(&index_mutexs[i]);
            if (buffer_matrix[i] < 1024)
            {
                pthread_mutex_unlock(&buffer_mutex);
                buffer_matrix[i]++;
                foundOne = TRUE;
                pthread_mutex_unlock(&index_mutexs[i]);
                break;
            }
            pthread_mutex_unlock(&index_mutexs[i]);
            if (i == shared_buffers - 1 && !foundOne)
            {
                pthread_mutex_unlock(&buffer_mutex);
                pthread_yield(NULL);
                i = 0;
                pthread_mutex_lock(&buffer_mutex);
            }
        }

        printf("producing #%d\n", current_items);
    }
}

void consumer(void *t)
{
    // nothing
}

int main(int argc, char **argv)
{
    int i = 0;
    int j = 0;
    long t = 0;

    if (argc < 5)
    {
        printf("Not enough arguments\ncorrect syntax: ./prodcon [# producer threads] "
            "[# consumer threads] [# shared buffers] [# items]\n");
        return 0;
    }

    char* arg = *(argv + 1);
    int producer_threads = atoi(arg);

    arg = *(argv + 2);
    int consumer_threads = atoi(arg);

    arg = *(argv + 3);
    shared_buffers = atoi(arg);

    buffer_matrix = (int *) malloc(sizeof(int *) * shared_buffers);
    index_mutexs =  (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t *) * shared_buffers);
    for(i = 0; i < shared_buffers; i++)
    {
        buffer_matrix[i] = 0;
        //pthread_mutex_init(&index_mutexs[i], NULL);
    }

    arg = *(argv + 4);
    max_items = atoi(arg);


    printf("creating %d producer threads, %d consumer threads, %d shared "
        "buffers, %d items to produce\n", producer_threads, consumer_threads
        , shared_buffers, max_items);


    pthread_t prod_threads[producer_threads];
    pthread_t con_threads[consumer_threads];

    for (i = 0; i < producer_threads; i++)
    {
        pthread_create(&prod_threads[i], NULL, (void *) &producer, (void *) t);
        t = t + 1;
    }

    t = 0;
    for (i = 0; i < consumer_threads; i++)
    {
        pthread_create(&con_threads[i], NULL, (void *) &consumer, (void *) t);
        t = t + 1;
    }

    for (i = 0; i < producer_threads; i++)
        {
                pthread_join(prod_threads[i], NULL);
        }
    for (i = 0; i < consumer_threads; i++)
        {
                pthread_join(con_threads[i], NULL);
        }

    printf("Good form pupper\n");

//  free(t);
    free(buffer_matrix);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

int max_items = 0;
int current_items_produced = 0;
int current_items_consumed = 0;
int* buffer_matrix;
int shared_buffers;

pthread_cond_t buffer_empty_cond;
pthread_mutex_t prod_count_mutex;
pthread_mutex_t con_count_mutex;
pthread_mutex_t buffer_mutex;
pthread_mutex_t* index_mutexs;

void print_buffer_status(int count)
{
    int i;
    pthread_mutex_lock(&buffer_mutex);
    for (i = 0; i < shared_buffers; i++)
    {
        pthread_mutex_lock(&index_mutexs[i]);
    }
    printf("__ buffer status __\ntotal count: %d\n", count);
    for (i = 0; i < shared_buffers; i++)
    {
        printf("index %d: %d\n", i, buffer_matrix[i]);
    }
    for (i = 0; i < shared_buffers; i++)
    {
        pthread_mutex_unlock(&index_mutexs[i]);
    }
    pthread_mutex_unlock(&buffer_mutex);
}

int producer_meets_conditions()
{
        pthread_mutex_lock(&prod_count_mutex);
        if (current_items_produced >= max_items)
        {
            pthread_mutex_unlock(&prod_count_mutex);
            return 0;
        }
        current_items_produced++;
        if (current_items_produced % 1000 == 0)
            print_buffer_status(current_items_produced);
        //printf("p: %d items out of %d\n", current_items_produced, max_items);
        return 1;
}

void producer(void *t)
{
    int i;
    while (producer_meets_conditions())
    {
        pthread_mutex_lock(&buffer_mutex);
        pthread_mutex_unlock(&prod_count_mutex);
        int foundOne = FALSE;
        for (i = 0; i < shared_buffers; i++)
        {
            //printf("p: %d loop %d\n", (int) t, i);
            pthread_mutex_lock(&index_mutexs[i]);
            if (buffer_matrix[i] < 1024)
            {
                pthread_mutex_unlock(&buffer_mutex);
                buffer_matrix[i]++;
                pthread_cond_signal(&buffer_empty_cond);
                foundOne = TRUE;
                //printf("%d producing #%d\n", (int) t, current_items_produced);
                pthread_mutex_unlock(&index_mutexs[i]);
                break;
            }
            pthread_mutex_unlock(&index_mutexs[i]);
            if (i == (shared_buffers - 1) && !foundOne)
            {
                pthread_mutex_unlock(&buffer_mutex);
                //cond here
                //printf("producer thread %d is yeilding\n", (int) t);
                i = -1;
                pthread_mutex_lock(&buffer_mutex);
            }
        }
    }
    printf("producer thread %d is finished\n", (int) t);
}

int consumer_meets_conditions()
{
    pthread_mutex_lock(&con_count_mutex);
    if (current_items_consumed >= max_items)
    {
        pthread_mutex_unlock(&con_count_mutex);
        return 0;
    }
    current_items_consumed++;
    //printf("c: %d items out of %d\n", current_items_consumed, max_items);
    return 1;
}

void consumer(void *t)
{
    int i;
    while (consumer_meets_conditions())
    {
        pthread_mutex_lock(&buffer_mutex);
        pthread_mutex_unlock(&con_count_mutex);
        int foundOne = FALSE;
        for (i = 0; i < shared_buffers; i++)
        {
            //printf("%d c: loop %d\n", (int) t, i);
            
            pthread_mutex_lock(&index_mutexs[i]);
            if (buffer_matrix[i] > 0)
            {
                pthread_mutex_unlock(&buffer_mutex);
                buffer_matrix[i]--;
                foundOne = TRUE;
                //printf("%d consuming #%d\n", (int) t, current_items_consumed);
                pthread_mutex_unlock(&index_mutexs[i]);
                break;
            }
            pthread_mutex_unlock(&index_mutexs[i]);
            //printf("DEBUG: i: %d\tfound: %d\n", i, foundOne);
            if (i == (shared_buffers - 1) && !foundOne)
            {
                printf("consumer thread %d is yeilding\n", (int) t);
                pthread_cond_wait(&buffer_empty_cond, &buffer_mutex);
                //pthread_yield(NULL);
                i = -1;
            }
        }
    }
    printf("consumer thread %d is finished\n", (int) t);
}

int main(int argc, char **argv)
{
    int i = 0;
    int j = 0;
    long t = 0;

    pthread_cond_init(&buffer_empty_cond, NULL);
    pthread_mutex_init(&buffer_mutex, NULL);
    pthread_mutex_init(&prod_count_mutex, NULL);
    pthread_mutex_init(&con_count_mutex, NULL);

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
    index_mutexs = malloc(sizeof(pthread_mutex_t) * shared_buffers);
    for(i = 0; i < shared_buffers; i++)
    {
        buffer_matrix[i] = 0;
        pthread_mutex_init(&index_mutexs[i], NULL);
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
        t++;
    }

    t = 0;
    for (i = 0; i < consumer_threads; i++)
    {
        pthread_create(&con_threads[i], NULL, (void *) &consumer, (void *) t);
        t++;
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

    pthread_cond_destroy(&buffer_empty_cond);
    pthread_mutex_destroy(&buffer_mutex);
    pthread_mutex_destroy(&prod_count_mutex);
    pthread_mutex_destroy(&con_count_mutex);
    for(i = 0; i < shared_buffers; i++)
    {
        pthread_mutex_destroy(&index_mutexs[i]);
    }
    free(buffer_matrix);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

void producer(void *t);
void consumer(void *t);

int max_items = 0;
int current_items = 0;
int* buffer_matrix;
int shared_buffers;

sem_t mutex;
sem_t empty;
sem_t full;

int main(int argc, char **argv)
{
	int i = 0;
	int j = 0;
	long t = 0;

	if (argc < 5)
	{
		printf("Not enough arguments\ncorrect syntax: ./prodcon [# producer threads] [# consumer threads] [# shared buffers] [# items]\n");
		return 0;
	}

	char* arg = *(argv + 1);
	int producer_threads = atoi(arg);

	arg = *(argv + 2);
	int consumer_threads = atoi(arg);

	arg = *(argv + 3);
	shared_buffers = atoi(arg);

	buffer_matrix = (int *) malloc(sizeof(int *) * shared_buffers);
	for(i = 0; i < shared_buffers; i++)
	{
		buffer_matrix[i] = 0;
	}

	arg = *(argv + 4);
	max_items = atoi(arg);

	sem_init(&mutex, 0, 1);
        sem_init(&full, 0, 0);
        sem_init(&empty, 0, 0);//max_items);

	printf("creating %d producer threads, %d consumer threads, %d shared buffers, %d items to produce\n", producer_threads, consumer_threads
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

//	free(t);
	free(buffer_matrix);
	sem_destroy(&mutex);
	sem_destroy(&full);
	sem_destroy(&empty);

	return 0;
}

void producer(void *t)
{
//	int current_count;
	while (TRUE)
	{
//		sem_getvalue(&full, &current_count);
		if (current_items >= max_items)
		{
			printf("exiting producer thread %d\n%d produced\n", (int) t, current_items);
			return;
		}
		if (sem_trywait(&mutex) == 0)
		{
//			printf("producing\n");
			current_items++;
			sem_post(&full);
			sem_post(&mutex);
		}
	}
}

void consumer(void *t)
{
	int current_count;
	int current_created;
	while (TRUE)
	{
		sem_getvalue(&full, &current_created);
		sem_getvalue(&empty, &current_count);
//		printf("empty val: %d\n", current_count);
		if (current_count >= max_items && current_created == 0)
		{
			printf("exiting consumer thread %d\n%d consumed\n", (int) t, current_count);
			return;
		}
		if (sem_trywait(&full) == 0)
		{
			if (sem_trywait(&mutex) == 0)
			{
//				printf("consuming\n");
				sem_post(&empty);
				sem_post(&mutex);
			}
			else
			{
				sem_post(&full);
			}
		}
	}
}

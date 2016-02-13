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
        sem_init(&empty, 0, max_items);

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
	int i = 0;
	while(TRUE)
	{
		sem_wait(&empty);
		if (current_items == max_items)
		{
			printf("Producer Thread %d is finished\n", (int)t);
			sem_post(&empty);
//			free(t);
			return;
		}
//		sem_wait(&empty);
		sem_wait(&mutex);
		//printf("entering prod\n");
		for (i = 0; i < shared_buffers; i++)
		{
			if (buffer_matrix[i] < 1023)
			{
				buffer_matrix[i]++;
				current_items++;
				break;
			}
		}
		sem_post(&mutex);
		sem_post(&full);
		if (current_items == max_items)
		{
			printf("shits broke yo\n");
			return;
		}
	}
}

// good form pupper
void consumer(void *t)
{
	int i = 0;
	int all_consumed = TRUE;
	while(TRUE)
        {
		sem_wait(&full);
		int current_count;
		sem_getvalue(&full, &current_count);
		printf("%d\n", current_count);
		if (current_count == max_items)
		{
			printf("Consumer Thread %d is finished\n", (int)t);
//			free(t);
			return;
		}
		all_consumed = TRUE;
//		sem_wait(&full);
//		sem_wait(&mutex);
//		printf("entering con\n");
		for (i = 0; i < shared_buffers; i++)
		{
			if (buffer_matrix[i] > 1)
			{
				all_consumed = FALSE;
				buffer_matrix[i]--;
				break;
			}
		}
		sem_post(&mutex);
		sem_post(&empty);
	}
}

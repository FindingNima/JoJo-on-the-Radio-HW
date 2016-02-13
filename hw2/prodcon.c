#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

void producer(void);
void consumer(void);

int max_items = 0;
int current_items = 0;
int** buffers_matrix;

int main(int argc, char **argv)
{
	int i = 0;

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
	int shared_buffers = atoi(arg);

	buffers_matrix = (int **) malloc(sizeof(int *) * shared_buffers);
	for (i = 0; i < shared_buffers; i++)
	{
		buffers_matrix[i] = (int *) malloc(sizeof(int) * max_items);
	}

	arg = *(argv + 4);
	max_items = atoi(arg);

	printf("creating %d producer threads, %d consumer threads, %d shared buffers, %d items to produce\n", producer_threads, consumer_threads
		, shared_buffers, items);


	pthread_t prod_threads[producer_threads];
	pthread_t con_threads[consumer_threads];


	for (i = 0; i < producer_threads; i++)
	{
		pthread_create(&prod_threads[i], NULL, (void *) &producer, NULL);
	}
	for (i = 0; i < consumer_threads; i++)
	{
		pthread_create(&con_threads[i], NULL, (void *) &consumer, NULL);
	}

	for (i = 0; i < producer_threads; i++)
        {
                pthread_join(prod_threads[i], NULL);
        }
	for (i = 0; i < consumer_threads; i++)
        {
                pthread_join(con_threads[i], NULL);
        }

	return 0;
}

void producer(void)
{
	while(TRUE)
	{
		if (current_items <= max_items)
		{
			

		}
		else
		{
			return;
		}
		printf("producing");
	}
}

void consumer(void)
{
	while(TRUE)
        {
                if (current_items <= max_items)
                {
			

                }
                else
                {
                       // check if all things are empty 
			return;
                }
		printf("consuming");
	}
}

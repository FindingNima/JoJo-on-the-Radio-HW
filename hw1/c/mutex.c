#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 100
#define TRUE 1
#define FALSE 0

void producer(void);
void consumer(void);
int produce_item();
void insert_item(int);
int remove_item();
void consume_item(int);

pthread_t threadies[2];
int count = 0;
int items[N]; 
pthread_mutex_t mutey;
pthread_cond_t condy;

void producer(void)
{
	int item;

	while(TRUE)
	{
		pthread_mutex_lock(&mutey);
		item = produce_item();
		while(count >= N) {
			pthread_cond_wait(&condy,&mutey);
		}
		insert_item(item);
		count = count + 1;
		if (count == 1) pthread_cond_broadcast(&condy);
		pthread_mutex_unlock(&mutey);
	}
}

void consumer(void)
{
	int item;

	while(TRUE)
	{
		pthread_mutex_lock(&mutey);
		while(count == 0) {
			pthread_cond_wait(&condy,&mutey);
		}
		item = remove_item();
		count = count - 1;
		consume_item(item);
		if (count == N-1) pthread_cond_broadcast(&condy);
		pthread_mutex_unlock(&mutey);
	}
}

int produce_item()
{
	int item = rand();
	printf("produced %d\n", item);
	return item;
}

void insert_item(int item)
{
	items[count] = item;
}

int remove_item()
{
	return items[count - 1];
}

void consume_item(int item)
{
	printf("consumed %d\n", item);
	return;
}

int main()
{

    pthread_t threads[2];
    long t;
    t = 1;
    pthread_create(&threads[0], NULL, (void *) &producer, (void *) t);
    t = 2;
    pthread_create(&threads[1], NULL, (void *) &consumer, (void *) t);
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
	return 0;
}
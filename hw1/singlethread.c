#include <stdio.h>
#define N 100
#define TRUE 1
#define FALSE 0

void producer(void);
void consumer(void);
int produce_item();
void insert_item(int);
int remove_item();
void consume_item(int);

int count = 0;
int items[N]; 

void producer(void)
{
	int item;

	while(TRUE)
	{
		item = produce_item();
		if (count == N)
			return;
		insert_item(item);
		count = count + 1;
	}
}

void consumer(void)
{
	int item;

	while(TRUE)
	{
		if (count == 0)
			return;
		item = remove_item();
		count = count - 1;
		consume_item(item);
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
	while (TRUE)
	{
		producer();
		consumer();
		printf("all consumed\n");
	}
	return 0;
}
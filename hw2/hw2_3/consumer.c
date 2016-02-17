#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <math.h>

#define FULL_SEM "full_sem"
#define EMPTY_SEM "empty_sem"
#define MUTEX_SEM "mutex_sem"

sem_t *full;
sem_t *empty;
sem_t *mutex;
int *file_memory;

void consumer() {

    int item;
    int intfull;
    int i = 0;
    while(i < 100)
    {
        sem_wait(full);
        sem_wait(mutex);
        sem_getvalue(full, &intfull);
        item = file_memory[intfull];
        printf("consumed %d\n", item);
        sem_post(mutex);
        sem_post(empty);
        i++;
    }
}


int main(int argc, char * argv[])
{
    full = sem_open(FULL_SEM, O_RDWR);
    empty = sem_open(EMPTY_SEM, O_RDWR);
    mutex = sem_open(MUTEX_SEM, O_RDWR); 
    file_memory = (int*)mmap(0, 100 * sizeof(int), PROT_WRITE, MAP_SHARED, atoi(argv[1]), 0);

    consumer();
        

    return 0;
}
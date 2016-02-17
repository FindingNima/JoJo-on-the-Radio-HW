#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define FULL_SEM "full_sem"
#define EMPTY_SEM "empty_sem"
#define MUTEX_SEM "mutex_sem"

sem_t *full;
sem_t *empty;
sem_t *mutex;
int *buffer;
void consumer() {

    int item;
    int intfull;
    int i = 0;
    while(i < 100)
    {
        sem_wait(full);
        sem_wait(mutex);
        sem_getvalue(full, &intfull);
        item = buffer[intfull];
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

    //ATTACH SHARED MEMORY SEGMENT
    buffer = (int *)shmat(atoi(argv[1]), NULL, 0);
    consumer();
        
    shmdt(buffer);
    return 0;
}
/*
 Example showing SHARED MEMORY
*/
#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h> 
#include <math.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h> 

#define FULL_SEM "full_sem"
#define EMPTY_SEM "empty_sem"
#define MUTEX_SEM "mutex_sem"
#define N 100

sem_t *full;
sem_t *empty;
sem_t *mutex;
int* buffer;
int status;

void producer() {


    int item;
    int intfull;
    int i = 0;
    while(i < 100)
    {
        item = rand();
        printf("produced %d\n", item);
        sem_wait(empty);
        sem_wait(mutex);
        sem_getvalue(full, &intfull);
        buffer[intfull] = item;
        sem_post(mutex);
        sem_post(full);
        i++;
    }
}




int main()
{
  full = sem_open(FULL_SEM, O_CREAT, 0644, 0); //tell how full the buffer is
  empty = sem_open(EMPTY_SEM, O_CREAT, 0644, N); //tell how empty the buffer is
  mutex = sem_open(MUTEX_SEM, O_CREAT, 0644, 1); // control access to critical region
  int segment_id;    //ID to Shared Memory Segment

    //ALLOCATE SHARED MEMORY SEGMENT
  segment_id = shmget(IPC_PRIVATE, 100 * sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
  const int ARG_SIZE = (int)((ceil(log10(segment_id))+1)*sizeof(char));
  char args[ARG_SIZE];
  sprintf(args, "%d", segment_id);
  buffer = (int*) shmat (segment_id, NULL, 0);
  producer();
  pid_t child = fork();
  char *argsConsumer[] = { "./consumer", args, NULL};


  if(child == 0){
     execvp("./consumer",argsConsumer);
  }
  waitpid(child, &status, WUNTRACED | WCONTINUED);
  shmctl(segment_id, IPC_RMID, 0);
  return 0;
}
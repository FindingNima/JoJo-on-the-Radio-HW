/*
 Example showing MEMORY MAPPED [WRITE TO MEMORY MAP]
*/

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
#define N 100

sem_t *full;
sem_t *empty;
sem_t *mutex;
int* file_memory;
int status;

void producer() {
    printf("WORKS SO FAR\n");
    int item;
    int intfull;
    int i = 0;
    while(i < 100)
    {
        item = rand();
        printf("produced %d\n", item);
        sem_wait(empty);
        sem_wait(mutex);
        //printf("WORKS SO FAR\n");
        sem_getvalue(full, &intfull);
        file_memory[intfull] = item;
        sem_post(mutex);
        sem_post(full);
        i++;
    }
}




int main()
{
  full = sem_open(FULL_SEM, O_CREAT, 0644, 0); 
  empty = sem_open(EMPTY_SEM, O_CREAT, 0644, N); 
  mutex = sem_open(MUTEX_SEM, O_CREAT, 0644, 1); 
  int fd;
  int result;

  fd = open("/home/ckfrancis/projects/JoJo-on-the-Radio-HW/hw2/hw2_3/swag.bin", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  result = lseek (fd, 101 * sizeof(int), SEEK_SET);  
  result = write(fd, "", 1); 

  file_memory = (int*)mmap(0, 100 * sizeof(int), PROT_WRITE, MAP_SHARED, fd, 0);

  const int ARG_SIZE = (int)((ceil(log10(fd))+1)*sizeof(char));
  char args[ARG_SIZE];
  sprintf(args, "%d", fd);

  producer();
  pid_t child = fork();
  char *argsConsumer[] = { "./consumer", args, NULL};


  if(child == 0){
     execvp("./consumer",argsConsumer);
  }
  waitpid(child, &status, WUNTRACED | WCONTINUED);
  munmap (file_memory, 101 * sizeof(int));
  close(fd);
  return 0;
}


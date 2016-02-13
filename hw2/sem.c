/*
 Functions needed for SEMAPHORE V
*/
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>



//Semaphore Union
union semun {
  int val;
  struct semid_ds *buf;
  unsigned short int *array;
  struct seminfo *__buf;
};

//Obtain binary Semaphore ID, Allocate if necessary
int binary_semaphore_allocation (key_t key, int sem_flags)
{
  return semget(key, 1, sem_flags);
}

//Deallocate binary Semaphore,  "return -1 if fail"
int binary_semaphore_deallocate (int semid)
{
  union semun ignored_argument;
  return semctl(semid,1,IPC_RMID, ignored_argument);
}

//INIT Binary Semaphore with Value 1
int binary_semaphore_initalize (int semid)
{
  union semun argument;
  unsigned short values[1];
  values[0]=1;
  argument.array=values;
  return semctl(semid,0,SETALL,argument);
}

/*******************************************
Wait and Post  --> Down and Up
*******************************************/

//WAIT
int binary_semaphore_wait(int semid)
{
  struct sembuf operations[1];

  //Use first semaphore --> Only one allocated at this point
  operations[0].sem_num=0;

  //Decrement semaphore by 1
  operations[0].sem_op = -1;

  //Permit undo  --> If process using sem terminates allows undoes any operation it made on the semaphore
  operations[0].sem_flg = SEM_UNDO;

  return semop(semid, operations, 1);
}


//POST
int binary_semaphore_post(int semid)
{
  struct sembuf operations[1];

  //Use first semaphore --> Only one allocated at this point
  operations[0].sem_num=0;

  //Increment semaphore by 1
  operations[0].sem_op = 1;

  //Permit undo
  operations[0].sem_flg = SEM_UNDO;

  return semop(semid, operations, 1);
}

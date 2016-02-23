#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
//#include <semaphore.h>
//#include <pthread.h>
 
#define TRUE 1
#define FALSE 0

void producer(FILE *pipe);
void consumer(FILE *pipe);

int main(int argc, char** argv) {
	
	pid_t prod_id, cons_id; //Is cons_id even needed?
	//int prod; //Need pipes to be initialized in an array
	//int cons;
	int pipes[2]; //1 read, 1 write
	FILE *pipeWrite, *pipeRead;
	
	//Inititalize our pipes
	pipe(pipes);
	pipeRead = fdopen(pipes[0], "r"); //Reading Pipe
	pipeWrite = fdopen(pipes[1], "w");//Writing Pipe
	
	//Forking stuff
	prod_id = fork();
	if(prod_id == 0) {
		fclose(pipeRead);
		producer(pipeWrite);
	}
//	else { //the child will be the consumer
	//	fclose(pipeWrite);
		//consumer(pipeRead);
	//}
	/* Not necessary
	else
	{
		perror("fork");
		exit(1);
	}*/
	
	cons_id = fork();
	if(consumer == 0) {
		fclose(pipeWrite);
		consumer(pipeRead);
	}
	/* Not necessary
	else
	{
		perror("fork");
		exit(1);
	}*/
	
	//free(pipeRead);
	//free(pipeWrite);
	//fclose(pipeRead);
	//fclose(pipeWrite);
	wait(NULL); //Used to wait for everything
	
	return 0;
}

void producer(FILE *pipe) {
	
	int i;
	
	//Keep producing, yo. Maybe start at 1?
	for(i = 0; i < 5000; i++) {
		fprintf(pipe, "%d", i);
		fflush(pipe); //jk go ahead and flush
		
	}
	
	//free(pipe);
	fclose(pipe);
	exit(0);
}

void consumer(FILE *pipe) {
	
	int i, j;
	
	//Always be consuming until there's no more
	while(TRUE) {
		i = fscanf(pipe, "%d", &j);
		//if(i == 0)
		if(i == 0) { //Still more to consume 
			printf("Consumer consumed %d\n", j);
		} else { //Consume last one
			break; //A few break statements never hurt anyone, right?
		}
	}
		
	//free(pipe);
	fclose(pipe);
	exit(0);
}
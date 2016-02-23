#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
//#include <netinet/in.h>
//#include <netdb.h>
#include <pthread.h>
#include <string.h>
 
#define TRUE 1
#define FALSE 0
//#define PORT 22222
#define PRODNAME "JoJoOnTheRadioProdSocket"
#define CONSNAME "JoJoOnTheRadioConsSocket"

void producer(void);
void consumer(void);
void* server(void*);
int serverChecker(int client_socket);
void write_to_server(int socket_fd, const char* text);
//void client(void);
//void consume(void);

int main(int argc, char** argv) {
	
	//Both the producer and consumer will be clients
	pid_t prod_id, cons_id; //Send to server 1 for prod and 0 for cons
	pthread_t serverThread; //Listening in for 1 and 0, send back total
	
	pthread_create(&serverThread, NULL, &server, NULL); //server function handles
	
	prod_id = fork();
	if(prod_id == 0){
		
		//stuff
		producer();
	}
	
	cons_id = fork();
	if(cons_id == 0) {
		//stuff
		consumer();
	}
	
	wait(NULL);
	pthread_join(serverThread, NULL);
	
	return 0;
}

void* server(void* unused) {
	int socket_fd;
	int client_sent_quit_message;
	struct sockaddr_un name;
	
	socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0); //localhost
	
	name.sun_family = AF_LOCAL;
	strcpy (name.sun_path, PRODNAME);
	bind(socket_fd, (struct sockaddr *)&name, SUN_LEN (&name));
	
	listen (socket_fd, 5);
	
	do {
		struct sockaddr_un client_name;
		socklen_t client_name_len = sizeof(struct sockaddr_un);
		int client_socket_fd;
		
		//accept connection
		client_socket_fd = accept(socket_fd, (struct sockaddr *)&client_name, &client_name_len);
		
		//sping off new server if client sends quit?
		client_sent_quit_message = serverChecker(client_socket_fd);
		
		close(client_socket_fd);
	} while(!client_sent_quit_message);
	
	close(socket_fd);
	unlink(PRODNAME);
	
	return;
}

int serverChecker(int client_socket) {
	while(TRUE) {
		int length;
		char* text;
		int stock_items = 0;
		
		//Read length of text message
		if(read(client_socket, &length, sizeof(length)) == 0) {
			return 0; //quit
		}
		
		text = (char*) malloc (length);
		read (client_socket, text, length);
		if(strcmp(text,"p")) {
			stock_items++;
			printf("Item Produced. Current Stock: %d\n", stock_items);
		} else {
			stock_items--;
			printf("Item Consumed. Current Stock: %d\n", stock_items);
			
			if(stock_items == 0) { //Everything is consumed, stop
				return 0;
			}
		}
		
		if(!strcmp(text,"quit")) {	
			free(text); //dealloc
			return 1;
		}
		
		free(text); //dealloc
	}
}

//Single thread, just keep producing & sending
void producer() {
	int socket_fd;
	struct sockaddr_un name;
	
	socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0);
	
	name.sun_family = AF_LOCAL;
	strcpy(name.sun_path, CONSNAME);
	
	connect(socket_fd, &name, SUN_LEN(&name));
	write_to_server(socket_fd, "p");
	
	close(socket_fd);
	
	return;
}

//Needs to be multi-threaded. One thread consumes, other reads & increments 
void consumer() {
	int socket_fd;
	struct sockaddr_un name;
	
	socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0);
	
	name.sun_family = AF_LOCAL;
	strcpy(name.sun_path, CONSNAME);
	
	connect(socket_fd, &name, SUN_LEN(&name));
	write_to_server(socket_fd, "c");
	
	close(socket_fd);
	
	return;
}

void write_to_server(int socket_fd, const char* text) {
	int length = strlen(text) + 1;
	write(socket_fd, &length, sizeof(length));
	write(socket_fd, text, length);
}
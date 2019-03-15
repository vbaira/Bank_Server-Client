#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include "operations.h"
#include "./taskQueue/taskQueue.h"
#include "./hashTable/hashTable.h"

#define BUFFER_SIZE 2048
#define perror2(s,e) fprintf(stdout,"%s : %s \n",s,strerror(e))

/*FUNCTION DECLARATIONS*/
void* thread_routine(void* arg);
void service_request(char* req,int sock);
void input_check(int argc,char* argv[],int* port,int* queue_size,int* pool_size);
void free_memory(struct HashTable* ht,struct TaskQueue* tq,pthread_t* pool);

/*GLOBAL VARIABLES*/
struct HashTable* ht;//global variable for hash table
struct TaskQueue* tq;//global variable for task queue

/*GLOBAL COND VARIABLES*/
pthread_cond_t taskQueue_nonempty;
pthread_cond_t taskQueue_full;


/*socket related code is taken from
class' presentation material*/
int main(int argc,char* argv[]){
	/*command line input check*/
	int port=-1;
	int queue_size=-1;
	int pool_size=-1;
	input_check(argc,argv,&port,&queue_size,&pool_size);
	//printf("queue:%d|port:%d|pool:%d\n",queue_size,port,pool_size);//
	/*create hashtable*/
	ht=HashTable_Create();
	/*create task queue*/
	tq=TaskQueue_Alloc(queue_size);
	/*initialize cond variables*/
	pthread_cond_init(&taskQueue_nonempty,NULL);
	pthread_cond_init(&taskQueue_full,NULL);
	/*create thread pool*/
	pthread_t* thread_pool;
	thread_pool=malloc(pool_size * sizeof(pthread_t));
	int err;
	int iter;
	for(iter=0; iter<pool_size; iter++){
		err=pthread_create( &thread_pool[iter], NULL,thread_routine,NULL);
		if( err != 0){
			perror2("error creating threads",err);
			exit(EXIT_FAILURE);
		}
	}
	/*create socket*/
	struct sockaddr_in server,client;
	socklen_t clientlen=sizeof(client);
	struct sockaddr* serverptr=(struct sockaddr*)&server;
	struct sockaddr* clientptr=(struct sockaddr*)&client;
	struct hostent* rem;
	int sock;
	if( (sock=socket(AF_INET,SOCK_STREAM,0)) < 0){
		perror("error creating socket");
		exit(EXIT_FAILURE);
	}
	server.sin_family=AF_INET; 
	server.sin_addr.s_addr=htonl(INADDR_ANY);
	server.sin_port=htons(port);
	/*bind socket*/
	if( bind(sock,serverptr,sizeof(server)) < 0){
		perror("error binding socket");
		exit(EXIT_FAILURE); 
	}
	/*listen for connections*/
	if( listen(sock,10) < 0){
		perror("error on listen");
		exit(EXIT_FAILURE);
	}
	pthread_mutex_t* queue_lock=TaskQueue_GetLock(tq);//get address of queue's lock to pass to cond_wait
	while(1){
		/*accept new connection*/
		int newsock;
		if( (newsock=accept(sock,clientptr,&clientlen)) < 0){
			perror("error on accept");
			exit(EXIT_FAILURE);		
		}
		TaskQueue_Lock(tq);//lock task queue
		/*block while task queue is full wait*/		
		while( TaskQueue_IsFull(tq)>0 ){
			pthread_cond_wait(&taskQueue_full,queue_lock);
		}
		/*queue is not full and we hold the lock*/		
		/*push new connection on task queue*/
		TaskQueue_Push(tq,newsock);
		TaskQueue_Unlock(tq);//unlock task queue
		/*broadcast that queue has at least on element*/
		pthread_cond_broadcast(&taskQueue_nonempty);
	}
}




/*thread routine*/
void* thread_routine(void* arg){
	pthread_mutex_t* queue_lock=TaskQueue_GetLock(tq);//get address of queue's lock to pass to cond_wait
	while(1){
		TaskQueue_Lock(tq);//lock task queue
		/*block while task queue is empty wait*/		
		while( TaskQueue_IsEmpty(tq)>0 ){
			pthread_cond_wait(&taskQueue_nonempty,queue_lock);
		}
		/*queue is not empty and we hold the lock*/
		/*pop  connection from task queue*/
		int sock=TaskQueue_Pop(tq);
		TaskQueue_Unlock(tq);//unlock task queue
		/*signal that queue is not full*/
		pthread_cond_signal(&taskQueue_full);
		/*start sommunicating with client on <sock>*/
		/*read size of request.
		if read returns 0 client closed socket,go to start of loop and
		wait for another client*/
		char buf[BUFFER_SIZE];
		int size;
		while( read(sock,&size,sizeof(int)) > 0 ){
				int bytes_left=size;
				int offset=0;
				/*read request*/
				while (bytes_left > 0){
					bytes_left=bytes_left-read(sock,buf+offset,bytes_left);
					offset=size-bytes_left;
				}
				/*manage request*/
				printf("<%u> %s",pthread_self(),buf);//
				service_request(buf,sock);					
		}
		printf("<%u> end of session\n",pthread_self());
		/*close socket*/
		close(sock);
	}
}


/*
decode request arguments and call the corresponding operation
to complete the request and respond to client
if there are not enough arguments ,the request is handled as an unknown 
command
*/
void service_request(char* req,int sock){
	char* saveptr;
	char* token;
	char* delimiters=" \n";
	token=strtok_r(req,delimiters,&saveptr);
	if( strcmp(token,"add_account")==0 ){
		int delay=-1;
		int arg_num=0;//argument number
		char name[100];
		int init_amount;
		while ( (token=strtok_r(NULL,delimiters,&saveptr)) != NULL ){
			if (arg_num==0){
				init_amount=atoi(token);
			}
			else if (arg_num==1){
				strcpy(name,token);
			}
			else if (arg_num==2){
				delay=atoi(token);	
			}
			else{
				break;
			} 
			arg_num++;
		}
		if (arg_num < 2){//less arguments,handle it as unkown command
			unknown_command(sock);
		}
		else{//sufficient arguments
			add_account(sock,name,init_amount,delay);					
		}
	}
	else if( strcmp(token,"add_transfer")==0 ){
		int delay=-1;
		int arg_num=0;//argument number
		char source[100];
		char dest[100];
		int amount;
		while ( (token=strtok_r(NULL,delimiters,&saveptr)) != NULL ){
			if (arg_num==0){
				amount=atoi(token);
			}
			else if (arg_num==1){
				strcpy(source,token);
			}
			else if (arg_num==2){
				strcpy(dest,token);
			}
			else if (arg_num==3){
				delay=atoi(token);
			}
			else{
				break;
			}
			arg_num++;
		}
		if (arg_num < 3){//less arguments,handle it as unkown command
			unknown_command(sock);
		}
		else{//sufficient arguments
			add_transfer(sock,source,dest,amount,delay);					
		}
	}
	else if( strcmp(token,"add_multi_transfer")==0 ){
		int delay=-1;
		int arg_num=0;//argument number
		char source[100];
		char* dest[50];//array to store destination accounts,i assume there wont be more than 50 						
		int amount;
		while ( (token=strtok_r(NULL,delimiters,&saveptr)) != NULL ){
			if(arg_num==0){//1st arg is amount
				amount=atoi(token);
			}
			else if(arg_num==1){//2nd is source account
				strcpy(source,token);
			}
			else{//all others are destination accounts
				dest[arg_num-2]=malloc((strlen(token)+1)*sizeof(char));
				strcpy(dest[arg_num-2],token);			
			}
			arg_num++;
		}
		if (arg_num < 3){//less arguments,handle it as unkown command
			unknown_command(sock);
		}
		else{//sufficient arguments
			/*check if last argument is delay*/
			int numOfDest=arg_num-2;
			/*if its a  number atoi  will convert it succesfully
			else it will return 0*/
			if (atoi(dest[numOfDest-1]) != 0){
				delay=atoi(dest[numOfDest-1]);
				numOfDest--;
			}
			add_multi_transfer(sock,source,amount,delay,dest,numOfDest);
		}
		int iter;
		/*free allocated memory*/
		for (iter=0;iter<arg_num-2;iter++){
			free(dest[iter]);
		}
	}
	else if( strcmp(token,"print_balance")==0 ){
		int delay=-1;
		char name[100];
		if( (token=strtok_r(NULL,delimiters,&saveptr)) != NULL ){
			strcpy(name,token);
			token=strtok_r(NULL,delimiters,&saveptr);
			if (token!=NULL) delay=atoi(token);
			print_balance(sock,name,delay);
		}
		else{//not enough arguments
			unknown_command(sock);
		}	
	}
	else if( strcmp(token,"print_multi_balance")==0 ){
		char* names[50];//array to store names,i assume there wont be more than 50 names 
		int balances[50];//array to fill with balances
		int delay=-1;
		int arg_num=0;
		while ( (token=strtok_r(NULL,delimiters,&saveptr)) != NULL ){
			names[arg_num]=malloc((strlen(token)+1)*sizeof(char));//allocate space to store name
			strcpy(names[arg_num],token);
			arg_num++;
		}
		if (arg_num < 1){//if there is not at least one name
			unknown_command(sock);
		}
		else{
			int numOfAccounts=arg_num;
			if (atoi(names[numOfAccounts-1]) != 0){
				delay=atoi(names[numOfAccounts-1]);
				numOfAccounts--;
			}
			print_multi_balance(sock,names,balances,numOfAccounts,delay);
		}
		int iter;
		/*free allocated memory*/
		for (iter=0;iter<arg_num;iter++){
			free(names[iter]);
		}
	}	
	else{//unkown command
		unknown_command(sock);
	}		
}


/*
check command line input and set queue_size,pool_size and port if everything is OK.
if an error occurs  print a message and exit proccess
*/
void input_check(int argc,char* argv[],int* port,int* queue_size,int* pool_size){
	/******Input check*******/
	if (argc != 7){
		printf("failure: application call should be something like:\n"
		"\t$ ./bankserver -p <port> -s <thread pool size> -q <queue size>\n");
		exit(EXIT_FAILURE);
	}
	/*analyze flags*/
	int iter;
	for (iter=0; iter<3; iter++){
		if( strcmp(argv[(2*iter)+1],"-s")==0 ) (*pool_size)=atoi(argv[(2*iter)+2]);
		else if ( strcmp(argv[(2*iter)+1],"-p")==0 ) (*port)=atoi(argv[(2*iter)+2]);
		else if ( strcmp(argv[(2*iter)+1],"-q")==0 ) (*queue_size)=atoi(argv[(2*iter)+2]);
		else{
			printf("failure:invalid flag\n");
			exit(EXIT_FAILURE);
		}
	}
	/*if one  variable was not set(one flag was used more than once) OR there was invalid input*/
	if ( (*pool_size<=0) || (*queue_size<=0) || (*port<=0) ){
		printf("failure: use each flag only once OR insert valid value for the flags\n");
		exit(EXIT_FAILURE);
	}		
}


/*
free allocated memory
*/
void free_memory(struct HashTable* ht,struct TaskQueue* tq,pthread_t* pool){
	HashTable_Destroy(ht);
	TaskQueue_Destroy(tq);
	free(pool);
}







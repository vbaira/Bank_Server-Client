#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include "./taskQueue/taskQueue.h"
#include "./hashTable/hashTable.h"

#define BUFFER_SIZE 2048

void bubble_sort(int* array,int numOfElements);

/*GLOBAL VARIABLES*/
extern struct HashTable* ht;//global variable for hash table
extern struct TaskQueue* tq;//global variable for task queue


/*
add_multi_transfer
*/
void add_multi_transfer(int sock,char* source,int amount,int delay,char** dest,int numOfDest){
	char response_buf[BUFFER_SIZE];
	int response_size=0;
	int status;
	int buckets[50];
	int iter;
	/*fill buckets array*/
	for (iter=0; iter<numOfDest; iter++){
		buckets[iter]=HashFunction(dest[iter]);
	}
	buckets[iter]=HashFunction(source);//add source bucket to array
	/*sort buckets' array*/
	bubble_sort(buckets,numOfDest+1);
	/*
	for (iter=0; iter<numOfDest+1; iter++){
		printf("%d\n",buckets[iter]);
	}*/
	/*lock buckets (in ascending order)*/
	for (iter=0; iter<numOfDest+1; iter++){
		HashTable_LockBucket(ht,buckets[iter]);
	}
	if (delay>0){
		usleep(1000*delay);//sleep for delay msec
	}
	status=HashTable_AddMultiTransfer(ht,source,dest,numOfDest,amount);
	/*unlock buckets*/
	for (iter=0; iter<numOfDest+1; iter++){
		HashTable_UnlockBucket(ht,buckets[iter]);
	}
	/*send  result to client*/
	if (status<0){//failure
		if (delay > 0){
			sprintf(response_buf,"Error. Multi-Transfer addition failed (%s:%d:%d)\n",source,amount,delay);
		}
		else{
			sprintf(response_buf,"Error. Multi-Transfer addition failed (%s:%d)\n",source,amount);
		}
	}
	else{//success
		if (delay > 0){
			sprintf(response_buf,"Success. Multi-Transfer addition (%s:%d:%d)\n",source,amount,delay);
		}
		else{
			sprintf(response_buf,"Success. Multi-Transfer addition (%s:%d)\n",source,amount);
		}
	}
	response_size=strlen(response_buf)+1;
	/*send size of response message*/
	if ( (write(sock,&response_size,sizeof(int))) == -1){
		perror ("Error in Writing");
		return; 
	}
	/*send response message*/
	int bytes_left=response_size;
	int offset=0;
	while (bytes_left > 0){
		bytes_left=bytes_left-write(sock,response_buf+offset,bytes_left);
		offset=response_size-bytes_left;
	}
}

/*
print multi balance handler
*/
void print_multi_balance(int sock,char** names,int* balance,int numOfAccounts,int delay){
	char response_buf[BUFFER_SIZE];
	int response_size=0;
	int status;
	int buckets[50];
	int iter;
	/*fill buckets array*/
	for (iter=0; iter<numOfAccounts; iter++){
		buckets[iter]=HashFunction(names[iter]);
	}
	/*sort buckets' array*/
	bubble_sort(buckets,numOfAccounts);
	/*
	for (iter=0; iter<numOfAccounts; iter++){
		printf("%d\n",buckets[iter]);
	}*/
	/*lock buckets (in ascending order)*/
	for (iter=0; iter<numOfAccounts; iter++){
		HashTable_LockBucket(ht,buckets[iter]);
	}
	if (delay>0){
		usleep(1000*delay);//sleep for delay msec
	}
	status=HashTable_PrintMultiBalance(ht,names,balance,numOfAccounts);
	/*unlock buckets*/
	for (iter=0; iter<numOfAccounts; iter++){
		HashTable_UnlockBucket(ht,buckets[iter]);
	}
	if (status<0){//failure
		strcpy(response_buf,"Error. Multi-Balance (");
		for (iter=0; iter<numOfAccounts; iter++){
			strcat(response_buf,names[iter]);
			strcat(response_buf,":");
		}
		if (delay > 0){
			char delay_str[20];
			sprintf(delay_str,"%d",delay);
			strcat(response_buf,delay_str);
			strcat(response_buf,":");
		}
		strcat(response_buf,"\b)\n");
	}
	else{//success
		char temp[50][20];//temp array to store balances as strings
		for (iter=0; iter<numOfAccounts; iter++){
			sprintf(temp[iter],"%d",balance[iter]);
		}
		strcpy(response_buf,"Success. Multi-Balance (");
		for (iter=0; iter<numOfAccounts; iter++){
			strcat(response_buf,names[iter]);
			strcat(response_buf,"/");
			strcat(response_buf,temp[iter]);
			strcat(response_buf,":");	
		}
		if (delay > 0){
			char delay_str[20];
			sprintf(delay_str,"%d",delay);
			strcat(response_buf,delay_str);
			strcat(response_buf,":");
		}
		strcat(response_buf,"\b)\n");
	}
	response_size=strlen(response_buf)+1;
	/*send size of response message*/
	if ( (write(sock,&response_size,sizeof(int))) == -1){
		perror ("Error in Writing");
		return; 
	}
	/*send response message*/
	int bytes_left=response_size;
	int offset=0;
	while (bytes_left > 0){
		bytes_left=bytes_left-write(sock,response_buf+offset,bytes_left);
		offset=response_size-bytes_left;
	}
}


/*
print balance handler
*/
void print_balance(int sock,char* name,int delay){
	char response_buf[BUFFER_SIZE];
	int response_size=0;
	int status;
	int bucket=HashFunction(name);
	int balance;
	/*lock bucket*/
	HashTable_LockBucket(ht,bucket);
	if (delay>0){
		usleep(1000*delay);//sleep for delay msec
	}
	/*get account balance*/
	balance=HashTable_PrintBalance(ht,name);
	/*unlock bucket*/
	HashTable_UnlockBucket(ht,bucket);
	if (balance<0){ //failure
		if (delay > 0){
			sprintf(response_buf,"Error. Balance (%s:%d)\n",name,delay);
		}
		else{
			sprintf(response_buf,"Error. Balance (%s)\n",name);
		}
	}
	else{//success
		if (delay > 0){
			sprintf(response_buf,"Success. Balance (%s:%d:%d)\n",name,balance,delay);
		}
		else{
			sprintf(response_buf,"Success. Balance (%s:%d)\n",name,balance);
		}	
	}
	response_size=strlen(response_buf)+1;
	/*send size of response message*/
	if ( (write(sock,&response_size,sizeof(int))) == -1){
		perror ("Error in Writing");
		return; 
	}
	/*send response message*/
	int bytes_left=response_size;
	int offset=0;
	while (bytes_left > 0){
		bytes_left=bytes_left-write(sock,response_buf+offset,bytes_left);
		offset=response_size-bytes_left;
	}
}

/*
add_transfer handler
*/
void add_transfer(int sock,char* source,char* dest,int amount,int delay){
	char response_buf[BUFFER_SIZE];
	int response_size=0;
	int status;
	int src_bucket=HashFunction(source);
	int dest_bucket=HashFunction(dest);
	/*lock buckets (in ascending order)*/
	if (src_bucket <= dest_bucket){
		HashTable_LockBucket(ht,src_bucket);
		HashTable_LockBucket(ht,dest_bucket);
	}
	else{
		HashTable_LockBucket(ht,dest_bucket);
		HashTable_LockBucket(ht,src_bucket);
	}
	if (delay>0){
		usleep(1000*delay);//sleep for delay msec
	}
	status=HashTable_AddTransfer(ht,source,dest,amount);
	/*unlock buckets(order doesnt matter)*/
	HashTable_UnlockBucket(ht,dest_bucket);
	HashTable_UnlockBucket(ht,src_bucket);
	/*send  result to client*/
	if (status<0){//failure
		if (delay > 0){
			sprintf(response_buf,"Error. Transfer addition failed (%s:%s:%d:%d)\n",source,dest,amount,delay);
		}
		else{
			sprintf(response_buf,"Error. Transfer addition failed (%s:%s:%d)\n",source,dest,amount);
		}
	}
	else{//success
		if (delay > 0){
			sprintf(response_buf,"Success. Transfer addition (%s:%s:%d:%d)\n",source,dest,amount,delay);
		}
		else{
			sprintf(response_buf,"Success. Transfer addition (%s:%s:%d)\n",source,dest,amount);
		}
	}
	response_size=strlen(response_buf)+1;
	/*send size of response message*/
	if ( (write(sock,&response_size,sizeof(int))) == -1){
		perror ("Error in Writing");
		return; 
	}
	/*send response message*/
	int bytes_left=response_size;
	int offset=0;
	while (bytes_left > 0){
		bytes_left=bytes_left-write(sock,response_buf+offset,bytes_left);
		offset=response_size-bytes_left;
	}
}



/*
add_account handler
*/
void add_account(int sock,char* name,int init_amount,int delay){
	char response_buf[BUFFER_SIZE];
	int response_size=0;
	int status;
	int bucket=HashFunction(name);
	/*lock bucket*/
	HashTable_LockBucket(ht,bucket);
	if (delay>0){
		usleep(1000*delay);//sleep for delay msec
	}
	status=HashTable_AddAccount(ht,name,init_amount);
	/*unlock bucket*/
	HashTable_UnlockBucket(ht,bucket);
	/*send  result to client*/
	if (status<0){//failure
		if (delay > 0){
			sprintf(response_buf,"Error. Account creation failed (%s:%d:%d)\n",name,init_amount,delay);
		}
		else{
			sprintf(response_buf,"Error. Account creation failed (%s:%d)\n",name,init_amount);
		}
	}
	else{//success
		if (delay > 0){
			sprintf(response_buf,"Success. Account creation (%s:%d:%d)\n",name,init_amount,delay);
		}
		else{
			sprintf(response_buf,"Success. Account creation (%s:%d)\n",name,init_amount);
		}
	}
	response_size=strlen(response_buf)+1;
	/*send size of response message*/
	if ( (write(sock,&response_size,sizeof(int))) == -1){
		perror ("Error in Writing");
		return; 
	}
	/*send response message*/
	int bytes_left=response_size;
	int offset=0;
	while (bytes_left > 0){
		bytes_left=bytes_left-write(sock,response_buf+offset,bytes_left);
		offset=response_size-bytes_left;
	}
}



/*
unkown command handler
*/
void unknown_command(int sock){
	char response_buf[BUFFER_SIZE];
	int response_size=0;
	strcpy(response_buf,"Error. Unknown command\n");
	response_size=strlen(response_buf)+1;
	/*send size of response message*/
	if ( (write(sock,&response_size,sizeof(int))) == -1){
		perror ("Error in Writing");
		return; 
	}
	/*send response message*/
	int bytes_left=response_size;
	int offset=0;
	while (bytes_left > 0){
		bytes_left=bytes_left-write(sock,response_buf+offset,bytes_left);
		offset=response_size-bytes_left;
	}
	printf("failure.unknown command\n\n");
	return;	
}


void bubble_sort(int* array,int numOfElements){
	int i,j,k,temp;
	for (i=0; i<numOfElements-1; i++){
		for (j=0; j<numOfElements-1-i; j++){
			if ( array[j] > array[j+1]){
				temp=array[j];
				array[j]=array[j+1];
				array[j+1]=temp;
			}
		}
	}
}


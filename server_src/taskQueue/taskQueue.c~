#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <error.h>

typedef struct TaskQueue{
	pthread_mutex_t lock;
	int* connection;
	int size;
	int start;
	int end;
	int count;
}TaskQueue;


TaskQueue* TaskQueue_Alloc(int q_size){
	TaskQueue* new=malloc(sizeof(TaskQueue));
	pthread_mutex_init(&(new->lock),NULL);
	new->size=q_size;
	new->start=0;
	new->end=-1;
	new->count=0;
	(new->connection)=malloc(q_size*sizeof(int));
}


pthread_mutex_t* TaskQueue_GetLock(TaskQueue* tq){
	return &(tq->lock);
}



void TaskQueue_Lock(TaskQueue* tq){
	pthread_mutex_lock(&(tq->lock));
}


void TaskQueue_Unlock(TaskQueue* tq){
	pthread_mutex_unlock(&(tq->lock));
}


int TaskQueue_IsFull(TaskQueue* tq){
	if ((tq->count) >= (tq->size)) return 1;//is full
	else return -1; // not full
}


int TaskQueue_IsEmpty(TaskQueue* tq){
	if ((tq->count) <= 0) return 1;//is empty
	else return -1; // not empty
}

/*
----BEFORE:TaskQueue_IsFull----
*/
int TaskQueue_Push(TaskQueue* tq,int fd){
	tq->end=((tq->end)+1) % (tq->size) ;
	tq->connection[(tq->end)]=fd;
	tq->count++;
	printf("New connection pushed at queue (fd:%d)\n\n",fd);
}

/*
----BEFORE:TaskQueue_IsEmpty----
*/
int TaskQueue_Pop(TaskQueue* tq){
	int fd=tq->connection[(tq->start)];
	tq->start=((tq->start)+1) % (tq->size) ;
	tq->count--;
	return fd;	
}


void TaskQueue_Destroy(TaskQueue* tq){
	free(tq->connection);
	free(tq);
}


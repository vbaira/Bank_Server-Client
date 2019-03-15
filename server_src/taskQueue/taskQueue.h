struct TaskQueue* TaskQueue_Alloc(int q_size);
int TaskQueue_IsFull(struct TaskQueue* tq);
int TaskQueue_IsEmpty(struct TaskQueue* tq);
int TaskQueue_Push(struct TaskQueue* tq,int fd);
int TaskQueue_Pop(struct TaskQueue* tq);
void TaskQueue_Destroy(struct TaskQueue* tq);
void TaskQueue_Lock(struct TaskQueue* tq);
void TaskQueue_Unlock(struct TaskQueue* tq);
pthread_mutex_t* TaskQueue_GetLock(struct TaskQueue* tq);

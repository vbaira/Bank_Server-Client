struct HashTable* HashTable_Create();
int HashFunction(char *str);
void HashTable_LockBucket(struct HashTable* ht,int bucket);
void HashTable_UnlockBucket(struct HashTable* ht,int bucket);
int HashTable_AddAccount(struct HashTable* ht,char* acc_name,int init_amount);
int HashTable_AddTransfer(struct HashTable* ht,char* src,char* dest,int amount);
int HashTable_AddMultiTransfer(struct HashTable* ht,char* src,char ** dest,int numOfDest,int amount);
int HashTable_PrintBalance(struct HashTable* ht,char* account);
int HashTable_PrintMultiBalance(struct HashTable* ht,char** account_name,int* balance,int numOfAccounts);

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "transactionList.h"



/*
Stuct definition
Each BucketChainNode describes an account
*/
typedef struct BucketChainNode{
	char name[100];
	int balance;
	struct TransactionList* transactions;
	struct BucketChainNode* next;
}BucketChainNode;


/*
head of bucket chain
*/
typedef struct BucketChain{
	BucketChainNode* start;
	int size;
}BucketChain;


/*
BucketChanin(head) allocation
*/
BucketChain* BucketChain_Alloc(){
	BucketChain* bc=malloc(sizeof(BucketChain));
	bc->start=NULL;
	bc->size=0;
	return bc;
}


/*
Bucket chain destroy
*/
void BucketChain_Destroy(BucketChain* bc){
	BucketChainNode* bcn=bc->start;
	BucketChainNode* todel;
	while (bcn != NULL){
		TransactionList_Destroy(bcn->transactions);
		todel=bcn;
		bcn=bcn->next;
		free(todel);
	}
	free(bc);
}




/*
get size of bucket chain
*/
int BucketChain_GetSize(BucketChain* bc){
	return bc->size;
}


/*
search if account exists.
return pointer to account if it exists.
return NULL otherwise
*/
BucketChainNode* BucketChain_Search(BucketChain* bc,char* acc_name){
	BucketChainNode* search=(bc->start);
	while (search != NULL){
		if (strcmp(acc_name,(search->name)) == 0){
			return search;//if found
		}
		else{
			search=(search->next);
		}
	}
	return NULL;
}


/*
add account.
if it exists,return -1
if not add the new account at the start of the chain and return 0
*/
int BucketChain_Add(BucketChain* bc,char* acc_name,int init_balance){
	BucketChainNode* search=BucketChain_Search(bc,acc_name);
	if(search==NULL){//if it doesnt exist
		BucketChainNode* new=malloc(sizeof(BucketChainNode));
		strcpy((new->name),acc_name);
		(new->balance)=init_balance;
		(new->next)=(bc->start);
		(new->transactions)=TransactionList_Alloc();
		(bc->start)=new;
		bc->size++;
		return 0;
	}
	else{//if it exists
		return -1;
	}
}

/*
get balance of account 
*/
int BucketChain_GetBalance(BucketChainNode* bcn){
	return bcn->balance;
}


/*
add_transfer
*/
void BucketChain_AddTransfer(BucketChainNode* src,BucketChainNode* dest,int amount){
	TransactionList_Add(dest->transactions,src->name,amount);
	src->balance-=amount;
	dest->balance+=amount;
}


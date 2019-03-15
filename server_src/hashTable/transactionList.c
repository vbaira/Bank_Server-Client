#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
Transaction list node
Each node describes an incoming transaction
*/
typedef struct TransactionListNode{
	char name[100];
	int trans_amount;
	struct TransactionListNode* next;
}TransactionListNode;


/*
head of transaction list
*/
typedef struct TransactionList{
	TransactionListNode* start;
	int size;
}TransactionList;


/*
transaction list allocation
*/
TransactionList* TransactionList_Alloc(){
	TransactionList* new=malloc(sizeof(TransactionList));
	new->start=NULL;
	new->size=0;	
	return new;
}

/*
TRANSACTION LIST DESTROY
*/
void TransactionList_Destroy(TransactionList* tl){
	TransactionListNode* tln=tl->start;
	TransactionListNode* todel;
	while (tln != NULL){
		todel=tln;
		tln=tln->next;
		free(todel);
	}
	free(tl);
}

/*
search if transaction exist.
return pointer to transaction if it exists.
return NULL otherwise
*/
TransactionListNode* TransactionList_Search(TransactionList* tl,char* from){
	TransactionListNode* search=(tl->start);
	while (search != NULL){
		if (strcmp(from,(search->name)) == 0){
			return search;//if found
		}
		else{
			search=(search->next);
		}
	}
	return NULL;//if not found
}


/*
add transaction.
if it exists,change trans_amount
if not add the new transaction at the start of the list
*/
void TransactionList_Add(TransactionList* tl,char* from,int trans){
	TransactionListNode* search=TransactionList_Search(tl,from);
	if(search==NULL){//if it doesnt exist
		TransactionListNode* new=malloc(sizeof(TransactionListNode));//allocate new node
		strcpy((new->name),from);
		(new->trans_amount)=trans;
		(new->next)=(tl->start);
		(tl->start)=new;
		tl->size++;
	}
	else{//if it exists
		search->trans_amount+=trans;	
	}
}






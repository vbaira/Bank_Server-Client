struct TransactionList* TransactionList_Alloc();
void TransactionList_Add(struct TransactionList* tl,char* from,int trans);
struct TransactionListNode* TransactionList_Search(struct TransactionList* tl,char* from);
void TransactionList_Destroy(struct TransactionList* tl);

struct BucketChain* BucketChain_Alloc();
int BucketChain_GetSize(struct BucketChain* bc);
struct BucketChainNode* BucketChain_Search(struct BucketChain* bc,char* acc_name);
int BucketChain_Add(struct BucketChain* bc,char* acc_name,int init_balance);
int BucketChain_GetBalance(struct BucketChainNode* bcn);
void BucketChain_AddTransfer(struct BucketChainNode* src,struct BucketChainNode* dest,int amount);
void BucketChain_Destroy(struct BucketChain* bc);

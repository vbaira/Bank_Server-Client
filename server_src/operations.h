void print_balance(int sock,char* name,int delay);
void print_multi_balance(int sock,char** names,int* balance,int numOfAccounts,int delay);
void add_account(int sock,char* name,int init_amount,int delay);
void add_transfer(int sock,char* source,char* dest,int amount,int delay);
void unknown_command(int sock);
void add_multi_transfer(int sock,char* source,int amount,int delay,char** dest,int numOfDest);

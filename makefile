OBJS1 =  hashTable.o bucketChain.o transactionList.o taskQueue.o operations.o server.o
OBJS2 =  client.o
SOURCE1 =  ./server_src/hashTable/hashTable.c ./server_src/hashTable/bucketChain.c ./server_src/hashTable/transactionList.c ./server_src/taskQueue/taskQueue.c ./server_src/operations.c ./server_src/server.c
SOURCE2 = ./client_src/client.c
OUT1 = bankserver
OUT2 = bankclient
CC = gcc
FLAGS = -g -c 
FLAGS2 = -g -c


all: $(OUT1) $(OUT2)

$(OUT1): $(OBJS1)
	$(CC) -g $(OBJS1) -o $(OUT1) -lpthread

hashTable.o: ./server_src/hashTable/hashTable.c
	$(CC) $(FLAGS) ./server_src/hashTable/hashTable.c -lpthread

bucketChain.o: ./server_src/hashTable/bucketChain.c
	$(CC) $(FLAGS) ./server_src/hashTable/bucketChain.c -lpthread

transactionList.o: ./server_src/hashTable/transactionList.c
	$(CC) $(FLAGS) ./server_src/hashTable/transactionList.c -lpthread

taskQueue.o: ./server_src/taskQueue/taskQueue.c
	$(CC) $(FLAGS) ./server_src/taskQueue/taskQueue.c -lpthread

operations.o: ./server_src/operations.c
	$(CC) $(FLAGS) ./server_src/operations.c -lpthread

server.o: ./server_src/server.c
	$(CC) $(FLAGS) ./server_src/server.c -lpthread


$(OUT2): $(OBJS2)
	$(CC) -g $(OBJS2) -o $(OUT2)

client.o: ./client_src/client.c
	$(CC) $(FLAGS2) ./client_src/client.c




# Bank_Server-Client
Bank server and client applications to demonstrate intersystem communication on a network via sockets and threads  

**Descriprion in greek**

## ΓΕΝΙΚΑ
Στο φάκελο με τα παραδοτέα   περιλαμβάνονται τα εξής αρχεία/φάκελοι:
- directory client_src:περιεχει το αρχειο client.c με το source code για τον bankclient

- directory server_src:περιεχει αρχεια με το source code για τον bankserver.Αναλυτικοτερα:
	- directory hashTable:περιεχει τα αρχεια hashTable.c/h bucketChain.c/h
	transactionList.c/h που περιεχουν τις δομες για την υλοποιηση του hashtable
	μαζι με ολες τις λειτουργειες τους.
	O κωδικας για την δομη του hashtable ειναι χωρισμενος σε επιπεδα.Το ανωτερο 
	επιπεδο με το οποιο επικοινωνει και ο bankserver ειναι αυτο του hashTable.To
	επομενο επιπεδο με το οποιο επικοινωνει το hashTable ειναι αυτο του 
	bucketChain που αναπαριστα τις λιστες σε καθε bucket του hashtable 
	και τελος το επιπεδο του transactionList που αναπαριστα τις λιστες με τις 
	συναλλαγες για καθε κομβο του bucketChain.

	- directory taskQueue:περιεχει τα αρχεια taskQueue.c/h που περιγραφουν την 
	δομη της ουρας συνδεσεων που εχει ο bankserver και τις λειτουργειες της.

	- αρχειο server.c : περιεχει την βασικη λογικη του bankserver

	- αρχεια operations.c/h: περιεχουν τις συναρτησεις που υλοποιουν την λογικη 
	των απαραιτητων λειτουργειων του server (add_account,add_transfer,
	add_multi_transfer,print_balance,print_multi_balance).

- αρχειο makefile : makefile για μεταγλωττιση.


## ΠΑΡΑΔΕΙΓΜΑ ΜΕΤΑΓΛΩΤΤΙΣΗΣ
Απο το directory με τα παραδοτέα οπου περιέχονται ολα τα παραπάνω η μεταγλώττιση γίνεται με μια απλή εντολή
~~~
make
~~~
Το makefile χρησιμοποιεί τα source files απο τα παραπάνω directories για να δημιουργήσει στο ιδιο επίπεδο  τα εκτελέσιμα bankserver και bankclient.


## ΠΑΡΑΔΕΙΓΜΑΤΑ ΕΚΤΕΛΕΣΗΣ
για εκτελεση του bankserver(οπως περιγραφεται στην εκφωνηση).Τα ζευγαρια ορισματων μπορουν να δινονται με οποιαδηποτε σειρα.
~~~
./bankserver -p <port> -s <thread_pool_size> -q <queue_size>
~~~

για εκτελεση του bankclient(οπως περιγραφεται στην εκφωνηση).Τα ζευγαρια ορισματων μπορουν να δινονται με οποιαδηποτε σειρα.
~~~
./bankclient -h <server_host> -p <server_port> -i <command_file>
~~~
**Περαιτερω οδηγιες για την λειτουργικοτητα και την χρηση των προγραμματων δινονται στο pdf της εκφωνησης του project.**

## ΣΗΜΕΙΑ ΠΡΟΣΟΧΗΣ
- Ο Κωδικας που αφορα τα sockets ειναι κατα κυριο λογο απο τις διαφανειες του μαθηματος με αλλαγες οπου χρειαζεται.

- Η συναρτηση κατακερματισμου που χρησιμοποιω ειναι η djb2 και την πηρα απο την παρακατω ιστοσελιδα:http://www.cse.yorku.ca/~oz/hash.html

- Η εντολη sleep στον client θεωρω πως ειναι προσθετικη,δηλαδη αμα δοθουν παραπανω απο μια sleep στην σειρα ο χρονος τους προστιθεται και η 
μεταδοση της επομενης εντολης  στον server καθυστερει για οσο χρονο εχει συσσωρευθει απο τις sleep.
πχ:
~~~
sleep 10  
add_account ... θα καθυστερησει την μεταδοση της add_account στον server για 10 msec.  
~~~
~~~
sleep 10  
sleep 20  
print_balance...θα καθυστερησει την μεταδοση της print_balance στον server για 30 msec.  
~~~

- Το πρωτοκολλο επικοινωνιας μεταξυ του server και του client ειναι το εξης.  
Ο client συνδεεται με τον server.
Υστερα για καθε εντολη που παιρνει ειτε απο το stdin ειτε  απο το input file (εξαιρουνται οι sleep και exit)
κανει write στο socket πρωτα το μεγεθος της εντολης και υστερα την ιδια την εντολη.Στην συνεχεια κανει blocking
read μεχρι να παρει το μεγεθος του response του server και το ιδιο το response.Αυτο συνεχιζεται μεχρι να παρει
exit.Οταν παρει exit κλεινει το socket απο την δικια του μερια και τερματιζει το process.
Αντιστοιχα καθε worker thread του server κανει wait οσο ειναι αδεια η ουρα των συνδεσεων.Οταν ερθει μια συνδεση 
το master thread του server την εισαγει στην ουρα συνδεσεων (εφοσον δεν ειναι γεματη,οταν ειναι γεματη το master thread Κανει wait)
και κανει broadcast οτι η ουρα δεν ειναι πλεον αδεια.Το καθε worker thread επιχειρει να κανει την συνδεση pop απο την ουρα
και αυτο που τα καταφερνει κανει signal οτι η ουρα δεν ειναι πλεον γεματη.Υστερα κανει blocking read μεχρι να παρει το μεγεθος 
του request απο τον client καθως και το ιδιο το request.Αυτο συνεχιζεται μεχρι το read που παιρνει το μεγεθος του request επιστρεψει 0,
που σημαινει οτι ο client εκλεισε το socket απο την μερια του.Οταν συμβει αυτο το worker thread κλεινει και αυτο το socket και
ξεκιναει απο την αρχη προσπαθωντας να εξυπηρετησει μια καινουργια συνδεση.


- Υποθετω οτι τα arguments στις εντολες add_multi_transfer , print_multi_balance ειναι μεχρι 50 στο πληθος.

- Οταν μια εντολη αν και valid εχει λιγοτερα απο τα απαιτουμενα arguments την χειριζομαι σαν unknown command.
πχ:για μια εντολη "add_account 100" που της λειπει το name o server θα επιστρεψει στον client "Error. Unknown command".

- Χρησιμοποιω ενα mutex ανα bucket γαι την δομη του hashTable.Καθε τετοιο mutex ειναι recursive για να αποφευγεται το self deadlock.

- Για την αποφυγη deadlock  τα buckets γινονται lock με αυξουσα σειρα.

- Ολες οι εντολες υποστηριζουν delay


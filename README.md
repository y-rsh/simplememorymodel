# simplememorymodel
reads specifically formatted c files and outputs memory model of trace of allocated memory in file.

---------------------------------

This is a simple memory model for c files formatted in a specific way. The way my program extracts memory allocation data from the input c file is by going through the file line by line and determining if memory allocation occurs. If it does,
the program then determines what type of memory allocation it is (is it allocated to the stack, heap etc) and based on that data (variable name, type, scope, size) adds a node containing said data to a linked list. Additionally, while iterating
through the lines in the input file the program keeps track of the total number of lines in the file, the number of lines per function, as well as the number of variables declared per function in the c file and adds this information to a 
secondary fileinfo linked list. Once these lists are populated (via the fetchdata function), the printdata() function formats and outputs the data in a presentable way, visualizing where in memory each of the variables in our c file are stored.   



---------------------------------
Functions:

void line(): prints '\n'

void breaker(): prints long series of dashes

node *create(): creates new linked list node

fnode *fcreate(): creates new fileinfo linked list node

node *append(): adds new linked list node with data corresponding to string inputs to list with start as head. If list is empty, initializes list.

fnode *appendf(): adds new linked list fileinfo node with data corresponding to integer inputs, adding to list with start as head. If list is empty, initializes list.

bool hasthepointer(): checks if a pointer has been previously declared to verify string literal allocation

bool isstringliteral(): uses hasthepointer to check if a specific line contains a string literal declaration

void findtype(): finds type corresponding to dynamic memory allocation by looking for previous variable declaration in the case where
	a pointer is declared on one line, and malloc is called on another line but on that pointer. Updates type to match found type in list.

void delete(): deletes list

void getsize(): checks input type and updates size to match corresponding size based on input type.

bool isvalidcfile(): checks if file with filename provided as input to function is a valid, existing, accessible c file.

bool ifunctionline(): checks if a line is a function declaration

bool ispointerline(): checks if a line is a pointer declaration

bool isarrayline(): checks if a line is an array declaration

bool isdynamic(): checks if a line contains a dynamic memory allocation (malloc)

int findsizeofvar(): finds the size of variable with specific name and scope in file

bool ismemoryallocation(): checks if a given line allocates memory in any form

node *fetchdata(): reads through each line in file and adds each memory allocation to linked list with append. Also adds file information (variables per function etc) to secondary linked list of fnodes
	and provides a pointer to the secondary linked list in the first node of the returned node linked list. Returns linked list containing all data necessary for printing

void printdata(): takes data formatted by fetchdata() and formats and prints the memory model for data read.



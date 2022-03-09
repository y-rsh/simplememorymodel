#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>


//definition of memory allocation linked list node


typedef struct FNNode{
    int lines;
    int numvars;
    int totallines;
    char funcname[48];
    struct FNNode *next;
}fnode;

typedef struct LLNode{
    char type[12];
    char name[24];
    char scope[24];
    char size[48];
    char position[12];
    struct LLNode *next;
    fnode *funcinfo;
}node;

//prints new line character
void line(){
    printf("\n");
}

//prints long string of dashes for print formatting
void breaker(){
    printf("-------------------------------------------\n");
}

//creates new node
node *create(){
    node *i = (node *)malloc(sizeof(node));
    return i;
}

//creates new fnode
fnode *fcreate(){
    fnode *i = (fnode *)malloc(sizeof(fnode));
    return i;
}

//adds new node with data given by char* arguments to linked list with start as head. If list is empty, initializes list.
node *append(node *start, char* type, char* name, char* scope, char* size, char* position){
    node *n = create();
    strcpy(n->type, type);
    strcpy(n->name, name);
    strcpy(n->scope, scope);
    strcpy(n->size, size);
    strcpy(n->position, position);
    n->next = NULL;
    if(start == NULL) return n;
    else{
        node *tpointer = start;
        while(tpointer->next != NULL){
            tpointer = tpointer->next;
        }
        tpointer->next = n;
        return start;
    }
}

/*
//prints flist
void printflist(fnode *start){
    fnode *fpointer = start;
    while(fpointer!=NULL){
        printf("%s: %d | %d | %d\n", fpointer->funcname, fpointer->lines, fpointer->numvars, fpointer->totallines);
        fpointer = fpointer->next;
    }
}
*/

//adds new fnode with data given by char* and int arguments to fnode linked list with start as head. If list is empty, initializes list.
fnode *appendf(fnode *start, char* funcname, int lines, int numvars, int totallines){
    fnode *n = fcreate();
    n->lines = lines;
    n->numvars = numvars;
    n->totallines = totallines;
    strcpy(n->funcname, funcname);
    n->next = NULL;
    if(start == NULL) return n;
    else{
        fnode *tpointer = start;
        while(tpointer->next != NULL){
            tpointer = tpointer->next;
        }
        tpointer->next = n;
        return start;
    }
}

//returns true if our current data linked list contains a pointer with name name, and whose scope has access to the current function we're in.
bool hasthepointer(char* name, char* scope, node *list){
    if(list == NULL) return false;
    node *tpointer = list;
    while(tpointer != NULL){
        if(strcmp(name, tpointer->name) == 0 && strcmp(scope, tpointer->scope) == 0){
            if(strstr(tpointer->type, "*") != NULL){
                return true;
            }
        }
        tpointer = tpointer->next;
    }
    return false;
}

//returns true if a line is a string literal declaration
bool isstringliteral(char* line, char* scope, node *list){
    char linecpy[strlen(line)];
    strcpy(linecpy, line);
    char* token = strtok(linecpy, " ");
    if(strstr(line, "*") != NULL && strstr(line, "\"") != NULL){
        if(strstr(line, "*") < strstr(line, "\"")){
            return(strstr(line, "=") != NULL);
        }
        else return false;
    }
    else if(strstr(line, "\"") != NULL){
        return hasthepointer(token, scope, list);
    }
    else return false;
}

//updates input type to the type corresponding to the type of node in linked list with name name and whose scope has access to current fn.
void findtype(char* name, char* scope, char* type, node *list){
    if(list == NULL) return;
    else{
        node* cpointer = list;
        while(cpointer!=NULL){
            if(strcmp(name, cpointer->name) == 0){
                if(strcmp(scope, cpointer->scope) == 0 || strcmp(scope, "global") == 0){
                    strcpy(type, cpointer->type);
                }
            }
            cpointer = cpointer->next;
        }
    }
    return;
}

//deletes list
void delete(node *list){
    if(list == NULL) return;
    else{
        while(list != NULL){
            node *prevpointer = list;
            list = list->next;
            free(prevpointer);
        }
    }
    return;
}

//deletes function info list
void fdelete(fnode *list){
    if(list == NULL) return;
    else{
        while(list != NULL){
            fnode *prevpointer = list;
            list = list->next;
            free(prevpointer);
        }
    }
    return;
}

//updates input size to match size given by type
void getsize(char* type, int typelength, char* size){
    //printf("type: %s\n", type);
    //printf("size: %s\n", size);
    //char size[48];
    int i;
    for(i = 0; i < strlen(type); i++){
        if(type[i] != ' ') break;
    }


    if(strstr(type, "*") != NULL){
        if(type[i] == 'c'){
            sprintf(size, "%d", (int)sizeof(char*));
        }
        else if(type[i] == 'i'){
            sprintf(size, "%d", (int)sizeof(int*));
        }
        else if(type[i] == 'f'){
            sprintf(size, "%d", (int)sizeof(float*));
        }
        else(sprintf(size, "err!"));
    }
    else{
        //printf("else statement hit\n");
        if(type[i] == 'c'){
            sprintf(size, "%d", (int)sizeof(char));
        }
        else if(type[i] == 'i'){
            sprintf(size, "%d", (int)sizeof(int));
        }
        else if(type[i] == 'f'){
            sprintf(size, "%d", (int)sizeof(float));
        }
        else(sprintf(size, "err!"));
    }
    //return size;
}

//checks if file given by argument is valid c file.
bool isvalidcfile(char* argument){
    if(access(argument, F_OK) == 0){
        if(argument[strlen(argument)-2] == '.' && argument[strlen(argument)-1] == 'c'){
            return 1;
        }
        else return(0);
    }
    else return(0);
}

//return true if line declares a function
bool isfunctionline(char* line){
    bool brackets = (strstr(line, "(") != NULL && strstr(line, ")") != NULL);
    char linecpy[strlen(line)];
    strcpy(linecpy, line);
    char* token = strtok(linecpy, " ");
    if(token == NULL) return 0;
    if(strcmp(token, "int") == 0 || strcmp(token, "float") == 0 || strcmp(token, "char") == 0 || strcmp(token, "void") == 0){
        return brackets;
    }
    else if(strstr(token, "int*") != NULL || strstr(token, "float*") != NULL || strstr(token, "char*") != NULL){
        return brackets;
    }
    return false;
}

//checks if a line declares a pointer
bool ispointerline(char* line, int size){
    char pointerchar[2];
    strcpy(pointerchar, "*");
    char linecpy[size];
    strcpy(linecpy, line);
    char* token = strtok(linecpy, " ");
    //printf("%s\n", token);
    if(strstr(token, "int") != NULL || strstr(token, "float") != NULL || strstr(token, "char") != 0) return (strstr(line, pointerchar) != NULL);
    else return 0;
}

//checks if a line declares an array
bool isarrayline(char* line){
    char close[2];
    char open[2];
    strcpy(close, "]");
    strcpy(open, "[");
    return ( strstr(line, open) && strstr(line, close));
}

//check if a line contains dynamic memory allocation
bool isdynamic(char* line){
    return(strstr(line, "malloc") != NULL);
}

//finds and returns the size of variable with name name and whose scope has access to current fn.
int findsizeofvar(node *list, char* name, char* scope){
    if(list == NULL) return -1;
    else{
        node *cpointer = list;
        while(cpointer != NULL){
            if(strcmp(cpointer->name, name) == 0){
                if(strcmp(cpointer->scope, scope) == 0 || strcmp(cpointer->scope, "global") == 0){
                    return atoi(cpointer->size);
                }
            }
            cpointer = cpointer->next;
        }
    }
    return -1;
}

//return true if line allocates memory
/*
memory allocation can take the following formats:
    int a;
    int *a;
    int a[];
    int a [];
    int* a;
    int * a;
    a = malloc();
    a = "string literal";
    a= malloc();
    a= "string literal";
    a =malloc();
    a ="string literal";xx cx
*/
bool ismemoryallocation(char* line){
    char linecpy[strlen(line)];
    strcpy(linecpy, line);
    char* token = strtok(linecpy, " ");
    if(token != NULL){
        if(strcmp("int", token) == 0){
            return true;
        }
        else if(strcmp("float", token) == 0){
            return true;
        }
        else if(strcmp("char", token) == 0){
            return true;
        }
        else if(strcmp("void", token) == 0){
            return true;
        }
        else if(strstr(token, "int*") != NULL){
            return true;
        }
        else if(strstr(token, "float*") != NULL){
            return true;
        }
        else if(strstr(token, "char*") != NULL){
            return true;
        }
        else{
            if(token == NULL) return false;
            if(token[strlen(token-1)] == '=' && (strstr(line, "\"") != NULL || strstr(line, "malloc") != NULL)){
                return true;
            }
            else{
                //printf("else statement hit\n");
                token = strtok(NULL, " ");
                if(token != NULL){
                    return (token[0] == '=' && (strstr(line, "\"") != NULL || strstr(line, "malloc") != NULL));
                }
                return false;
            }
        }
    }
    else return false;
}

/*
int findsizeof(node* list, char* name, char* scope){
    if(list == NULL) return -1;
    else{
        node *cpointer = list;
        while(cpointer != NULL){
            if(strcmp(cpointer->name, name) == 0){
                if(strcmp(cpointer->scope, "global") == 0 || strcmp(cpointer->scope, scope) == 0){
                    if(strstr(cpointer->size, "*") != NULL){
                        char* token;
                        int size = 0;
                        char sizecpy[sizeof(cpointer->size)];
                        strcpy(sizecpy ,cpointer->size);
                        token = strtok(sizecpy, " *");
                        size += atoi(token);
                        token = strtok(NULL, " *");
                        size *= atoi(token);
                    }
                }
            }
            cpointer = cpointer->next;
        }
    }
}
*/

//reads through input file and populates linked list with new memory information, returns pointer to new LL
node *fetchdata(FILE* filestream, char* filename){
    //printf("fetch data called \n");
    char type[12];
    char name[24];
    char scope[24];
    char size[48];
    //char position[12];
    char line[256];
    char linecpy[256];
    char alphanumstr[256];
    strcpy(alphanumstr, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
    char *token;
    node *list = NULL;
    fnode *flist = NULL;
    bool function = false;
    int totallines = 0;
    int funclines = 0;
    int funcvars = 0;
    if(isvalidcfile(filename)){
        strcpy(scope, "global");
        while(fgets(line, sizeof line, filestream) != NULL){
            totallines++;
            funclines++;
            strcpy(linecpy, line);
            //printf("linecpy made successfully\n");

            char marker = line[0];
            if(line[0] == '{') function = true;
            if(line[0] == '}'){
                //printf("reached { line\n");
                //printf("scope: %s | funclines: %d | funcvars: %d\n", scope, funclines, funcvars);
                flist = appendf(flist, scope, funclines, funcvars, 0);
                funclines = 0;
                funcvars = 0;
                function = false;
                strcpy(scope, "global");
            }

            //printf("checking mem allocation:\n");
            if(ismemoryallocation(line)){
                //printf("memory allocation detected at line: \n");
                //printf("%s\n", line);

                //add memory allocation for function declaration line:
                if(isfunctionline(line)){
                    bool emptyfn = false;
                    char linecpycpy[256];
                    //printf("is fn line called \n");
                    token = strtok(linecpy, " *(");
                    token = strtok(NULL, " *(");

                    //printf("token is now: %s\n", token);

                    //update scope to function we're in
                    strcpy(scope, token);

                    funclines = 0;
                    funcvars = 0;

                    //strcpy(position, "stack");

                    //reinitialize line copy to extract data from brackets
                    strcpy(linecpy, line);

                    //token is first word
                    token = strtok(linecpy, "()");
                    //printf("token is now: %s\n", token);

                    //token is interior of func brackets:
                    token = strtok(NULL, "()");
                    //printf("token is now: %s\n", token);

                    //popularize linecpycpy with contents of function brackets
                    //i.e. void func(<type1> <name1>, <type2> <name2>, <type3> <name3>):
                    //linecpy should be "<type1> <name1>, <type2> <name2>, <type3> <name3>".
                    strcpy(linecpycpy, token);
                    emptyfn = (strlen(linecpycpy) == 1);
                    //printf("linecpycpy is now: %s\n", linecpycpy);
                    //printf("length of linecpycpy  is: %lu\n", strlen(linecpycpy));

                    if(!emptyfn){
                        token = strtok(linecpycpy, ",");
                        char tokencpy[strlen(token)];
                        char* toktoken;

                        //printf("token before loop is: %s\n", token);



                        //iterate through type / name pairs and extract data:
                        while(token != NULL){
                            //printf("current token is: %s\n", token);
                            strcpy(tokencpy, token);
                            int n;
                            for(n = 0; n < strlen(tokencpy); n++){
                                if(isalnum(tokencpy[n])) break;
                            }
                            for(n = n; n < strlen(tokencpy); n++){
                                if(tokencpy[n] == ' ') break;
                            }
                            for(n = n; n < strlen(tokencpy); n++){
                                if(isalnum(tokencpy[n])) break;
                            }

                            strcpy(name, &tokencpy[n]);

                            tokencpy[n] = '\0';
                            strcpy(type, tokencpy);
                            tokencpy[n]=' ';
                            
                            //toktoken = strtok(tokencpy, " ");

                            //updating type of variable to add to linked list
                            //strcpy(type, toktoken);

                            //updating size of variable to add to linked list
                            getsize(type, strlen(type), size);
                            
                            

                            list = append(list, type, name, scope, size, "stack");
                            funcvars++;

                            token = strtok(NULL, ",");
                            //printf("token is now (after loop): %s\n", token);
                        }
                    }

                }

                //add memory allocation for static (global) variables;
                else if(function == false){
                    //printf("is static fn called for line:\n");
                    //printf("%s\n", line);
                    bool namebool = 0;
                    //token = strtok(linecpy, " *;");

                    //get i as index of first occurrence of alphanumeric outside of type declaration
                    int i;
                    int n = 0;
                    for(i = 0; i < strlen(line); i++){
                        if(namebool == 0){
                            if(line[i] == ' '){
                                namebool = 1;
                            }
                        }
                        else{
                            //first time we hit alphanumeric, i.e. first time we hit specific name
                            //printf("about to check for alphanum\n");
                            if(isalnum(line[i])){
                                break;
                            }
                        }
                    }


                    //copy characters up to name of variables into "type" field.
                    char a = line[i];
                    line[i] = '\0';
                    strcpy(type, line);
                    //printf("type updated to: %s\n", type);

                    strcpy(linecpy, line);
                    line[i] = a;

                    //printf("about to get size\n");

                    //initialize token to first word in line.
                    //token = strtok(linecpy, " ,*;");


                    getsize(linecpy, strlen(linecpy), size);

                    strcpy(linecpy, line);

                    //printf("token got\n");

                    token = strtok(linecpy, " ,*;\n");
                    //printf("token initialized to %s\n", token);
                    //read first variable name
                    token = strtok(NULL, " ,*;\n");
                    while(token != NULL){
                        //printf("token is: %s\n", token);
                        if(token[0] == '=') break;
                        strcpy(name, token);
                        //printf("about to call append \n");
                        list = append(list, type, name, scope, size, "static");
                        //printf("append called\n");
                        token = strtok(NULL, " ,*;\n");
                    }
                    //printf("loop exited\n");
                }

                //add memory allocation for string literals (R.O. Memory):
                else if(isstringliteral(line, scope, list)){
                    funcvars++;
                    //printf("is literal called \n");
                    //case where full declaration of literal on one line:
                    //i.e. char* c = "string literal"
                    if(strstr(line, "*") != NULL){
                        strcpy(type, "char*");
                        sprintf(size, "%lu", sizeof(char*));

                        token = strtok(linecpy, " *");
                        token = strtok(NULL, " *");

                        strcpy(name, token);

                        append(list, type, name, scope, size, "stack");

                        strcpy(linecpy, line);
                        token = strtok(linecpy, "\"");
                        token = strtok(NULL, "\"");
                        sprintf(size, "%d * %lu", (int)strlen(token), sizeof(char));
                        strcpy(type, "char[]");

                        append(list, type, name, scope, size, "ROM");

                        //strcpy(size, getsize(type, strlen(type)));  
                    }

                    //case where string literal is assigned to previous pointer declaration:
                    //i.e. 
                    //char* c;
                    //c = "string literal";
                    else{
                        //get name of variable:
                        token = strtok(linecpy, " ");
                        strcpy(name, token);

                        //reinitialize line and get string length for size:
                        strcpy(linecpy, line);
                        token = strtok(linecpy, "\"");
                        token = strtok(NULL, "\"");
                        sprintf(size, "%d * %lu", (int)strlen(token), sizeof(char));


                        strcpy(type, "char[]");
                        append(list, type, name, scope, size, "ROM");
                    }
                }

                //add memory allocation for dynamic variables (malloc, calloc):
                else if(isdynamic(line)){
                    funcvars++;
                    //printf("is dynamic called\n");
                    //case where full variable allocated and declared here:
                    //i.e. char* c = malloc(sizeof(something));
                    bool namebool = 0;
                    if(strstr(line, "*") != NULL){
                        int i;
                        for(i = 0; i < strlen(line); i++){
                            if(namebool == 0){
                                if(line[i] == ' '){
                                    namebool = 1;
                                }
                            }
                            else{
                                //first time we hit alphanumeric, i.e. first time we hit specific name
                                if(isalnum(line[i])){
                                    break;
                                }
                            }
                        }

                        //copy characters up to name of variables into "type" field.
                        char a = line[i];
                        line[i] = '\0';
                        strcpy(type, line);
                        //strcpy(token, line);
                        line[i] = a;

                        token = strtok(linecpy, " *");
                        token = strtok(NULL, " *");
                        strcpy(name, token);

                        token = strtok(NULL, "()");
                        strcpy(size, token);
                        
                        append(list, type, name, scope, size, "heap");
                    }
                    //case where only variable allocated here
                    //i.e.:
                    //char* c;
                    //c = malloc(sizeof(something));
                    else{
                        token = strtok(linecpy, " ");
                        strcpy(name, token);
                        findtype(name, scope, type, list);
                        token = strtok(NULL, "(");
                        //token[strlen(token) - 2] = '\0';
                        token = strtok(NULL, ";");
                        token[strlen(token) - 1] = '\0';
                        strcpy(size, token);
                        append(list, type, name, scope, size, "heap");
                    }
                }

                //add memory allocation for stack variables:
                //should all take one of the following forms:
                //<type> <*> <name1>, <name2>, <name3>, ... ;
                //<type> <name1><[]>;
                else{
                    funcvars++;
                    //printf("stack line called \n");
                    if(isarrayline(line)){
                        //printf("is array line\n");
                        token = strtok(linecpy, " ");
                        strcpy(type, token);

                        //printf("token is now: %s\n", token);
                        token = strtok(NULL, " ");
                        char tempstring[sizeof(line)];

                        //printf("token is now: %s\n", token);

                        //array declaration right after name (no space)
                        if(strstr(token, "[") != NULL){
                            strcpy(tempstring, token);
                            token = strtok(tempstring, "[]");
                            token = strtok(NULL, "[]");

                        }
                        else{
                            token = strtok(NULL, " ");
                            token[strlen(token)-1] = '\0';
                            token = token + 1;
                        }
                        if(isdigit(token[0])){
                            //int n = atoi(token);
                            if(type[0] == 'i'){
                                sprintf(size, "%s * %lu", token, sizeof(int));
                            }
                            else if(type[0] == 'f'){
                                sprintf(size, "%s * %lu", token, sizeof(float));
                            }
                            else if(type[0] == 'c'){
                                sprintf(size, "%s * %lu", token, sizeof(char));
                            }
                        }
                        //inside array brackets is variable name of integer
                        else{
                            int n = findsizeofvar(list, token, scope);
                            if(type[0] == 'i'){
                                sprintf(size, "%s * %lu",token, sizeof(int));
                            }
                            else if(type[0] == 'f'){
                                sprintf(size, "%s * %lu",token, sizeof(float));
                            }
                            else if(type[0] == 'c'){
                                sprintf(size, "%s * %lu",token, sizeof(char));
                            }
                        }
                        strcpy(linecpy, line);
                        token = strtok(linecpy, " [");
                        token = strtok(NULL, " [");
                        strcpy(name, token);

                        append(list, type, name, scope, size, "stack");
                    }

                    else{
                        //printf("is not array line\n");
                        bool namebool = 0;
                        int n;
                        for(n = 0; n < strlen(line); n++){
                            if(line[n] != ' ') break;
                        }
                        int i;
                        for(i = n; i < strlen(line); i++){
                            if(namebool == 0){
                                if(line[i] == ' '){
                                    namebool = 1;
                                }
                            }
                            else{
                                //first time we hit alphanumeric, i.e. first time we hit specific name
                                if(isalnum(line[i])){
                                    //printf("line[%d] is first alnum: %c", i, line[i]);
                                    break;
                                }
                            }
                        }

                        //copy characters up to name of variables into "type" field.
                        char a = line[i];
                        line[i] = '\0';
                        strcpy(type, line);

                        //printf("type is now: %s\n", type);
                        //strcpy(token, line);
                        line[i] = a;


                        if(ispointerline(line, sizeof(line))){
                            if(type[n] == 'i'){
                                sprintf(size, "%lu", sizeof(int*));
                            }
                            else if(type[n] == 'f'){
                                sprintf(size, "%lu", sizeof(float*));
                            }
                            else if(type[n] == 'c'){
                                sprintf(size, "%lu", sizeof(char*));
                            }
                        }
                        else{
                            if(type[n] == 'i'){
                                sprintf(size, "%lu", sizeof(int));
                            }
                            else if(type[n] == 'f'){
                                sprintf(size, "%lu", sizeof(float));
                            }
                            else if(type[n] == 'c'){
                                sprintf(size, "%lu", sizeof(char));
                            }
                        }

                        //printf("size has been updated to %s\n", size);

                        token = strtok(linecpy, " *,;=\n");
                        //printf("token is: %s\n", token);

                        token = strtok(NULL, " *,;=\n");
                        //printf("token is: %s\n", token);

                        while(token != NULL){
                            if(strcmp(token, "=") == 0) break;
                            append(list, type, token, scope, size, "stack");
                            token = strtok(NULL, " *,;\n");
                            //printf("token is: %s\n", token);
                        }
                    }
                }
            }
            //printf("end of single loop reached\n");
        }
        flist = appendf(flist, scope, -1, -1, totallines);
        //printflist(flist);
        list->funcinfo = flist;
        //printf("returning list\n");
        return list;
    }
    else return NULL;
}

//prints data read through fetchdata.
void printdata(node *n){
    node *cpointer = n;
    printf("*** R.O. Data ***       scope  type  size\n");
    line();
    //cpointer = n;
    while(cpointer != NULL){
        if(strcmp(cpointer->position, "ROM") == 0){
            printf("%s : %s | %s | %s \n", cpointer->name, cpointer->scope, cpointer->type, cpointer->size);
        }
        cpointer = cpointer->next;
    }
    cpointer = n;
    breaker();
    line();
    printf("*** Static (Global) Data ***     scope  type  size\n");

    while(cpointer != NULL){
        if(strcmp(cpointer->position, "static") == 0){
            printf("%s : %s | %s | %s \n", cpointer->name, cpointer->scope, cpointer->type, cpointer->size);
        }
        cpointer = cpointer->next;
    }

    cpointer = n;
    breaker();
    line();
    printf("*** Dynamic (HEAP) Data ***     scope  type  size\n");

    while(cpointer != NULL){
        if(strcmp(cpointer->position, "heap") == 0){
            printf("%s : %s | %s | %s \n", cpointer->name, cpointer->scope, cpointer->type, cpointer->size);
        }
        cpointer = cpointer->next;
    }

    cpointer = n;
    breaker();
    line();
    printf("###############################\n");
    printf("######## UNUSED SPACE #########\n");
    printf("###############################\n");
    line();

    breaker();
    printf("*** Stack ***            scope  type  size\n");
    line();

    while(cpointer != NULL){
        if(strcmp(cpointer->position, "stack") == 0){
            printf("%s : %s | %s | %s \n", cpointer->name, cpointer->scope, cpointer->type, cpointer->size);
        }
        cpointer = cpointer->next;
    }
    breaker();

    printf("*** ADDITIONAL STATISTICS: ***\n");
    breaker();
    printf("Variables Per Function: \n");
    line();
    fnode *fpointer = n->funcinfo;

    while(fpointer != NULL){
        if(fpointer->lines != -1){
            printf("%s : %d\n", fpointer->funcname, fpointer->numvars);
        }
        fpointer = fpointer->next;
    }
    printf("------------\n");
    printf("Lines Per Function: \n");
    line();

    fpointer = n->funcinfo;

    while(fpointer != NULL){
        if(fpointer->lines != -1){
            printf("%s : %d\n", fpointer->funcname, fpointer->lines);
        }
        fpointer = fpointer->next;
    }

    printf("------------\n");
    printf("Total Number of Functions: ");

    fpointer = n->funcinfo;

    int i = 0;

    while(fpointer->next != NULL){
        if(fpointer->lines != -1){
            i++;
            //printf("%s : %d\n", fpointer->funcname, fpointer->lines);
        }
        fpointer = fpointer->next;
    }

    printf("%d\n", i);

    printf("------------\n");

    printf("Total Number of Lines: %d\n", fpointer->totallines);

    breaker();

}

int main(int argc, char** argv){
    if(argc != 2){
        fprintf(stderr, "Please Enter 1 FileName.\n");
        return 0;
    }
    else if(!isvalidcfile(argv[1])){
        fprintf(stderr, "Please Input Valid C File.\n");
        return 0;
    }
    else{
        FILE *myfile = fopen(argv[1], "r");
        breaker();
        printf("MEMORY TRACE:\n");
        breaker();
        printf("### exec / text ###\n");
        printf("%s\n", argv[1]);
        line();
        breaker();
        node *n = fetchdata(myfile, argv[1]);
        printdata(n);
        fdelete(n->funcinfo);
        delete(n);
    }
}
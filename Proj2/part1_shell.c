#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

//echo "hello world" | ls -a --author

struct Node {
    char *str;
    char *tokenType;
    struct Node* next;
};

void createLinkedListAndPrint(char** argv) {
    char *cmd[64];
    int j = 0;

    struct Node* head = malloc(sizeof(struct Node));
    head->str = argv[0];
    head->next = NULL;
    head->tokenType = "Command";
    cmd[j] = head->str;
    j++;
    
    int idx = 1;
    struct Node* temp = head;
    while(argv[idx] != NULL) {
        struct Node* newNode = malloc(sizeof(struct Node));
        temp->next = newNode;
        temp->next->str = argv[idx];
        temp->next->next = NULL;
        
        if(temp->next->str[1] == '-') {
            temp->next->tokenType = "FLAG2";
        } else if(temp->next->str[0] == '-') {
            temp->next->tokenType = "FLAG1";
        } else if (temp->next->str[0] == '|' || temp->next->str[0] == '<' || temp->next->str[0] == '>' || temp->next->str[0] == '&') {
            temp->next->tokenType = "Symbol";
        } else if(temp->tokenType == "Symbol" && temp->str[0] == '|') {
            temp->next->tokenType = "Command";
            cmd[j] = temp->next->str;
            j++;
        } else {
            temp->next->tokenType = "ARG";
        }
        //printf("%s\n", temp->str);
        temp = temp->next;
        idx++;
    }

    struct Node* tail = malloc(sizeof(struct Node));
    temp->next = tail;
    tail->str = '\0';
    //cmd[j+1] = '\0'

    printf("END\n");

    printf("Commands: ");
    for(int iter = 0; iter < j; iter++) {
        printf("%s, ", cmd[iter]);
    }

    temp = head;
    while(temp->str != NULL) {
        if(temp->tokenType == "Command") {
            printf("\n%s : ", temp->str);
        } else if (temp->tokenType != "Symbol"){
            printf("%s ", temp->str);
        }
        temp = temp->next;
    }

    printf("\n----------------------------------------------------\n");
    temp = head;
    while(temp->str != NULL) {
        printf("Token: %s\tType: %s\n", temp->str, temp->tokenType);
        temp = temp->next;
    }

    if(temp->str == NULL) {
        printf("\nNULL\n");
    }

}

void parse(char *line, char **argv)
{
    while (*line != '\0') {       /* if not the end of line ....... */ 
        while (*line == ' ' || *line == '\t' || *line == '\n')
            *line++ = '\0';       /* replace white spaces with 0    */
        if (*line != '\0')
            *argv++ = line;       /* save the argument position     */
        while (*line != '\0' && *line != ' ' && 
               *line != '\t' && *line != '\n') 
            line++;               /* skip the argument until ...    */
    }
    *argv = NULL;                 /* mark the end of argument list  */
}

int main(void)
{
    char line[1024];             /* the input line                 */
    char *argv[64];              /* the command line argument      */

    while (1) {                   /* repeat until done ....         */
        fputs("Shell -> ",stdout);     /*   display a prompt             */
        fgets(line, 1024, stdin);              /*   read in the command line     */
        fputs("\n", stdout);
        parse(line, argv);       /*   parse the line               */
        if (strcmp(argv[0], "exit") == 0)  /* is it an "exit"?     */
            exit(0);            /*   exit if it is               */

        createLinkedListAndPrint(argv);


        //execute(argv);           /* otherwise, execute the command */
    }


    return 0;
}

// void execute(char **argv) //write your code here
// {

// //HINT you need to introduce the following functions: fork, execvp, waitpid
// //Advanced HINT: pipe, dup2, close, open

//      pid_t  pid;
//      int    status;

//      pid = fork();
//      if (pid == 0) {
//           // In child process
//           //char *argv[] = {"echo", "child:", "THIS", "IS", "ECHO", NULL };
//           execvp(argv[0], argv);
//           printf("exec failed!\n");
//           exit(1);
//     } else if (pid > 0) {
//           printf("parent: waiting ...\n");
//           //waitpid(pid, &status, WEXITSTATUS(status));
//           waitpid(pid, &status, 0);
//           //wait(&status);
//           printf("parent: the child exited with status %d\n", WEXITSTATUS(status));
//     } else {
//           perror("fork failed");
//           exit(1);
//     }
// }



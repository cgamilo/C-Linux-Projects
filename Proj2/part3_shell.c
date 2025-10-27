#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

//echo "hello world" | ls -a --author

struct Node {
    char *str;
    char *tokenType;
    struct Node* next;
    struct Node* prev;
};

struct Node* createLinkedListAndPrint(char** argv, char** cmd) {
    //char *cmd[64];
    int j = 0;

    struct Node* head = malloc(sizeof(struct Node));
    head->str = argv[0];
    head->next = NULL;
    head->tokenType = "Command";
    head->prev = NULL;
    cmd[j] = head->str;
    j++;
    
    int idx = 1;
    struct Node* temp = head;
    while(argv[idx] != NULL) {
        struct Node* newNode = malloc(sizeof(struct Node));
        temp->next = newNode;
        temp->next->str = argv[idx];
        temp->next->next = NULL;
        temp->next->prev = temp;
        
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
    temp->next->prev = temp;
    tail->str = '\0';
    //cmd[j+1] = '\0'

    //printf("END\n");

    // printf("Commands: ");
    // for(int iter = 0; iter < j; iter++) {
    //     printf("%s, ", cmd[iter]);
    // }

    // temp = head;
    // while(temp->str != NULL) {
    //     if(temp->tokenType == "Command") {
    //         printf("\n%s : ", temp->str);
    //     } else if (temp->tokenType != "Symbol"){
    //         printf("%s ", temp->str);
    //     }
    //     temp = temp->next;
    // }

    // printf("\n----------------------------------------------------\n");
    // temp = head;
    // while(temp->str != NULL) {
    //     printf("Token: %s\tType: %s\n", temp->str, temp->tokenType);
    //     temp = temp->next;
    // }

    // if(temp->str == NULL) {
    //     printf("\nNULL\n");
    // }

    return head;
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

void rightArrrowExec(struct Node* temp) {
    //printf("=== dup2 File Redirection ===\n");

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return;
    } else if (pid == 0) {
        // Child process: redirect output to file
        int fd = open(temp->next->str, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open failed");
            exit(1);
        }
        
        //printf("This message goes to terminal (before redirection)\n");
        
        // Redirect stdout to file
        dup2(fd, STDOUT_FILENO);
        close(fd);
        
        struct Node* findCmd = temp;
        
        while(findCmd->tokenType != "Command") {
            findCmd = findCmd->prev;
            //printf("%s\n", findCmd->str);
        }
        char *cmd = findCmd->str;
        //printf("Cmd: %s\n", cmd);

        
        //char *args_1[] = {"ls", "-l", NULL, "garbage"};
        char *args_1[64];
        int args_idx = 0;
        while(findCmd != temp) {
            if(findCmd->tokenType != "Symbol") {
                args_1[args_idx] = findCmd->str;
                //printf("%s\n", findCmd->str);
                args_idx++;
            }
            findCmd = findCmd->next;
        }
        args_1[args_idx] = NULL;
        
        // printf("Command Arguments: ");
        // printf("%s\n", args_1[0]);
        // printf("%s\n", args_1[1]);
        // if(args_1[2] == NULL) {
        //     printf("fuck");
        // } else {
        //     printf("%s\n", args_1[2]);
        // }
        

        execvp(cmd, args_1);
        exit(0);
    } else {
        // Parent waits and then displays file contents
        waitpid(pid, NULL, 0);
        // printf("Child completed. File contents:\n");
        
        // // Display what was written to the file
        // FILE *file = fopen(temp->next->str, "r");
        // if (file) {
        //     char buffer[256];
        //     while (fgets(buffer, sizeof(buffer), file)) {
        //         printf("File: %s", buffer);
        //     }
        //     fclose(file);
        //     //unlink("output.txt");  // Clean up
        // }
    }
    printf("\n");
}

void leftArrrowExec(struct Node* temp) {
    //printf("=== dup2 File Redirection ===\n");

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return;
    } else if (pid == 0) {
        // Child process: redirect output to file
        int fd = open(temp->next->str, O_RDONLY);
        if (fd == -1) {
            perror("open failed");
            exit(1);
        }
        
        //printf("This message goes to terminal (before redirection)\n");
        
        // Redirect stdout to file
        dup2(fd, STDIN_FILENO);
        close(fd);
        
        struct Node* findCmd = temp;
        while(findCmd->tokenType != "Command") {
            findCmd = findCmd->prev;
        }
        char *cmd = findCmd->str;
        //printf("\nCommand: %s\n", cmd);
        
        //char *args_1[] = {"cat", NULL};
        char *args_1[64];
        int args_idx = 0;
        while(findCmd != temp) {
            if(findCmd->tokenType != "Symbol") {
                args_1[args_idx] = findCmd->str;
                //printf("%s\n", findCmd->str);
                args_idx++;
            }
            findCmd = findCmd->next;
        }
        args_1[args_idx] = NULL;

        // printf("Command Arguments: ");
        // for(int i = 0; i <= args_idx; i++) {
        //     printf("%s\n", args_1[i]);
        // }
        
        execvp(cmd, args_1);
        exit(0);
    } else {
        // Parent waits and then displays file contents
        waitpid(pid, NULL, 0);
        //printf("Child completed. File contents:\n");
        
        // Display what was written to the file
        // FILE *file = fopen(temp->next->str, "r");
        // if (file) {
        //     char buffer[256];
        //     while (fgets(buffer, sizeof(buffer), file)) {
        //         printf("File: %s", buffer);
        //     }
        //     fclose(file);
        //     //unlink("output.txt");  // Clean up
        // }
    }
    printf("\n");
}

void ampersandExec(struct Node* temp) {

    struct Node* findCmd = temp;
    while(findCmd->tokenType != "Command") {
        findCmd = findCmd->prev;
    }
    char *cmd = findCmd->str;
    //printf("\nCommand: %s\n", cmd);
    
    //char *args_1[] = {"cat", NULL};
    char *args_1[64];
    int args_idx = 0;
    while(findCmd != temp) {
        if(findCmd->tokenType != "Symbol") {
            args_1[args_idx] = findCmd->str;
            //printf("%s\n", findCmd->str);
            args_idx++;
        }
        findCmd = findCmd->next;
    }
    args_1[args_idx] = NULL;

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        execvp(cmd, args_1);
        printf("exec failed!\n");
        exit(1);
    }
}

void pipeExec(struct Node* temp) {
    int pipefd[2];
    pid_t pid1, pid2;

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return;
    }

    struct Node* findCmd1 = temp;
    while(findCmd1->tokenType != "Command") {
        findCmd1 = findCmd1->prev;
    }
    char *cmd1 = findCmd1->str;

    struct Node* findCmd2 = temp;
    while(findCmd2->tokenType != "Command") {
        findCmd2 = findCmd2->next;
    }
    char *cmd2 = findCmd2->str;
    //printf("\nCommand: %s\n", cmd);
    
    //char *args_1[] = {"cat", NULL};
    char *args_1[64];
    int args1_idx = 0;
    while(findCmd1 != temp) {
        if(findCmd1->tokenType != "Symbol") {
            args_1[args1_idx] = findCmd1->str;
            //printf("%s\n", findCmd->str);
            args1_idx++;
        }
        findCmd1 = findCmd1->next;
    }
    args_1[args1_idx] = NULL;

    char *args_2[64];
    int args2_idx = 0;
    while(findCmd2 != NULL) {
        if(findCmd2->tokenType != "Symbol") {
            args_2[args2_idx] = findCmd2->str;
            //printf("%s\n", findCmd->str);
            args2_idx++;
        }
        findCmd2 = findCmd2->next;
    }
    args_2[args2_idx] = NULL;

    // First child: execute 'ls'
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork failed");
        return;
    } else if (pid1 == 0) {
        // Child 1: ls
        close(pipefd[0]);  // Close read end
        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to pipe
        close(pipefd[1]);
        //char *args_ls[] = {"ls", NULL};
        execvp(cmd1, args_1);
        perror("execvp failed");
        exit(1);
    }

    // Second child: execute 'wc -l'
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork failed");
        return;
    } else if (pid2 == 0) {
        // Child 2: wc -l
        close(pipefd[1]);  // Close write end
        dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin from pipe
        close(pipefd[0]);
        //char *args_wc[] = {"wc", "-l", NULL};
        execvp(cmd2, args_2);
        perror("execvp failed");
        exit(1);
    }

    // Parent closes both ends and waits
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    printf("\n");
}

void execute(struct Node* head, char **argv, char **cmd) //write your code here
{

//HINT you need to introduce the following functions: fork, execvp, waitpid
//Advanced HINT: pipe, dup2, close, open

     pid_t  pid;
     int    status;

    int symbolBool = 0;
    struct Node* temp = head;
    while(temp->str != NULL) {
    if(temp->str[0] == '|') {
        //printf("SYMBOL DETECTED\n");
        pipeExec(temp);
        symbolBool = 1;
    } else if(temp->str[0] == '<') {
        //printf("SYMBOL DETECTED\n");
        leftArrrowExec(temp);
        symbolBool = 1;
    } else if(temp->str[0] == '>') {
        //printf("SYMBOL DETECTED\n");
        rightArrrowExec(temp);
        symbolBool = 1;
    } else if(temp->str[0] == '&') {
        //printf("SYMBOL DETECTED\n");
        ampersandExec(temp);
        symbolBool = 1;
    }
    temp = temp->next;
    }
    if(symbolBool == 0) {
        pid = fork();
        if (pid == 0) {
            // In child process
            //char *argv[] = {"echo", "child:", "THIS", "IS", "ECHO", NULL };
            execvp(head->str, argv);
            printf("exec failed!\n");
            exit(1);
        } else if (pid > 0) {
            //printf("parent: waiting ...\n");
            //waitpid(pid, &status, WEXITSTATUS(status));
            waitpid(pid, &status, 0);
            //wait(&status);
            //printf("parent: the child exited with status %d\n", WEXITSTATUS(status));
        } else {
            perror("fork failed");
            exit(1);
        }
    }
    
 
}

int main(void)
{
    char line[1024];             /* the input line                 */
    char *argv[64];              /* the command line argument      */
    char *cmd[64];

    while (1) {                   /* repeat until done ....         */
        fputs("Shell -> ",stdout);     /*   display a prompt             */
        fgets(line, 1024, stdin);              /*   read in the command line     */
        fputs("\n", stdout);
        parse(line, argv);       /*   parse the line               */
        if (strcmp(argv[0], "exit") == 0)  /* is it an "exit"?     */
            exit(0);            /*   exit if it is               */
        
        struct Node* head = createLinkedListAndPrint(argv, cmd);
        execute(head, argv, cmd);           /* otherwise, execute the command */
    }


    return 0;
}





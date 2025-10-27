#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

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

void  execute(char **argv) //write your code here
{

//HINT you need to introduce the following functions: fork, execvp, waitpid
//Advanced HINT: pipe, dup2, close, open

     pid_t  pid;
     int    status;

     pid = fork();
     if (pid == 0) {
          // In child process
          //char *argv[] = {"echo", "child:", "THIS", "IS", "ECHO", NULL };
          execvp(argv[0], argv);
          printf("exec failed!\n");
          exit(1);
    } else if (pid > 0) {
          printf("parent: waiting ...\n");
          //waitpid(pid, &status, WEXITSTATUS(status));
          waitpid(pid, &status, 0);
          //wait(&status);
          printf("parent: the child exited with status %d\n", WEXITSTATUS(status));
    } else {
          perror("fork failed");
          exit(1);
    }
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
            exit(0);            /*   exit if it is                */
        execute(argv);           /* otherwise, execute the command */
    }
    return 0;
}

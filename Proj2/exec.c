#include <stdio.h>
#include <unistd.h>

int main() {
    //char *argv[] = { "echo", "this", "is", "echo", NULL };
    //char *argv[] = {"grep", ".c", ".", NULL};
    char *argv[] = { "ls", NULL };
    execvp("ls", argv);
    //execvp("echo", argv);
    // If execvp is successful, the following line will not be execu ted
    printf("execvp failed\n");
    return 0;
}
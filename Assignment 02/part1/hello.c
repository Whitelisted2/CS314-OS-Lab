#include <stdio.h>
#include <unistd.h> // for getpid(), getppid(), etc.
#include <stdlib.h> // contains rand()
#include <sys/wait.h> // for wait()

int main()
{
    char str[] = "Hello World";
    int pid = 0;
    int t;
    printf("\n|letter|current_pid|parent_pid|\n");
    for(int i=0; i<11; i++){
        if(pid == 0){   // if child is being executed, only then enter the block to spawn new child
            t = rand()%4 + 1;
            sleep(t);       
            pid = fork();
            if(pid == 0){   // child prints the letter
                printf("|%-6c|%-11d|%-10d|\n", str[i], (int)getpid(), (int)getppid());
            }
            wait(NULL); // parent waits for child to execute, then child is terminated
        }
    }
    return 0;
}
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char* argv[]){
    if(argc == 1){
        exit(0);
    }
    int num = atoi(argv[argc-1]);   // get the input number
    int result = (int)num/2;             // compute result

    printf("Half: Current process id: %d, Current result: %d\n", (int)getpid(), (int)result);

    char* nextcmd[argc];            // next command would have (argc - 1) arguments + 1 numerical input
    for(int i=0; i<argc; i++){
        nextcmd[i] = argv[i+1];     // omit first argument
    }

    sprintf(nextcmd[argc-2], "%d", result); // result as a string placed in the nextcmd
    //nextcmd[argc-1] = NULL;                 // next cmd is smaller than current one by 1; precaution

    // for(int i=0; i<argc-1; i++){ // testing block
    //     printf("%s\n", nextcmd[i]);
    // }
    execvp(nextcmd[0], nextcmd);
    return 0;
}
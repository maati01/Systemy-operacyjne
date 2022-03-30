#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void input_verification(int argc, char* argv){
    if(argc != 2){
        printf("Wrong input!\n");
        exit(0);
    }
    if(atoi(argv) == 0){
        printf("Wrong input!\n");
        exit(0);
    }
}

int main(int argc, char** argv){
    input_verification(argc, argv[1]);
    int n = atoi(argv[1]);
    pid_t pid;

    for(int i = 0; i < n; i++) {
        pid = fork();
        if(pid < 0) {
            printf("Error");
            exit(1);
        } else if (pid == 0) {
            printf("Number of process: %d\n", i + 1);
            printf("Current pid: %d\n", getpid());
            exit(0); 
        } else  {
            wait(0);
        }
    }

    return 0;
}
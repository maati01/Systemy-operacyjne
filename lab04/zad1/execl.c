#include <stdio.h>
#include <signal.h>
#include <string.h>

int main(int argc, char** argv){
    if(strcmp(argv[1], "ignore") == 0){
        printf("[CHILD] SIGUSR1 ignored\n");
        raise(SIGUSR1);
    }else if (strcmp(argv[1], "mask") == 0)
    {
        printf("[CHILD] SIGUSR1 masked\n");
        raise(SIGUSR1);

    }else if (strcmp(argv[1], "pending") == 0)
    {
        sigset_t curr_mask;
        sigemptyset(&curr_mask);
        printf("[CHILD] the signal list has been read: %s\n", sigpending(&curr_mask) == 0 ? "true" : "false");
        printf("[CHILD] SIGUSR1 pending: %s\n", sigismember(&curr_mask, SIGUSR1) ? "yes" : "no");
                
    }
    return 0;
}
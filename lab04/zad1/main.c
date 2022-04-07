#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

//SIGUSR1 - sygnal zdefiniowany przez uzytkownika

void signal_handler(int signum){
    printf("Signal received!\n");
}

int main(int argc, char** argv){
    
    if(strcmp(argv[1], "ignore") == 0){
        signal(SIGUSR1, SIG_IGN);
        raise(SIGUSR1);
        printf("[PARENT] SIGUSR1 ignored\n");

    }else if (strcmp(argv[1], "handler") == 0)
    {
        signal(SIGUSR1, signal_handler);
        raise(SIGUSR1);
    }else if (strcmp(argv[1], "mask") == 0)
    {  
        sigset_t newmask;/* sygnały do blokowania */
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        printf("[PARENT] signal blocked: %s\n", sigprocmask(SIG_BLOCK, &newmask, NULL) < 0 ? "false" : "true");
        printf("[PARENT] SIGUSR1 masked\n");
        raise(SIGUSR1);

    }else if (strcmp(argv[1], "pending") == 0)
    {
        sigset_t newmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        printf("[PARENT] signal blocked: %s\n", sigprocmask(SIG_BLOCK, &newmask, NULL) < 0 ? "false" : "true");
        raise(SIGUSR1);
        printf("[PARENT] signal list read: %s\n", sigpending(&newmask) == 0 ? "true" : "false");
        printf("[PARENT] SIGUSR1 pending: %s\n", sigismember(&newmask, SIGUSR1) ? "yes" : "no");
    }

    if(strcmp(argv[2], "fork") == 0){
        pid_t child = fork();
        if(child == 0){
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
                printf("[CHILD] signal list read: %s\n", sigpending(&curr_mask) == 0 ? "true" : "false");
                printf("[CHILD] SIGUSR1 pending: %s\n", sigismember(&curr_mask, SIGUSR1) ? "yes" : "no");
            }
        }
        else{
			wait(0);
		}
    }
    else{
        execl("./execl", "./execl", argv[1], NULL);
    }

    return 0;
}
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int signals_back = 0;
int wait = 1;
int COUNT_SIGNAL = SIGUSR1;
int END_SIGNAL = SIGUSR2;
pid_t sender_pid;

void send_handler(int sig, siginfo_t *info, void *ucontext) {
    if (sig != COUNT_SIGNAL)
        return;
    wait = 1;
    signals_back++;
}

void finish_handler(int sig, siginfo_t *info, void *ucontext) {
    if (sig != END_SIGNAL)
        return;
    wait = 0;
    sender_pid = info->si_pid;;
}

int main(int argc, char** argv){
    char* mode = argv[1];

    printf("Catcher PID: %d\n", getpid());

    if(strcmp(mode, "sigrt") == 0){
        COUNT_SIGNAL = SIGRTMIN;
        END_SIGNAL = SIGRTMAX;
    }

    struct sigaction act;
    act.sa_sigaction = send_handler;
    sigfillset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(COUNT_SIGNAL, &act, NULL);

    struct sigaction act_finish;
    act_finish.sa_sigaction = finish_handler;
    sigfillset(&act_finish.sa_mask);
    act_finish.sa_flags = SA_SIGINFO;
    sigaction(END_SIGNAL, &act_finish, NULL);

    sigset_t newmask;
    sigemptyset(&newmask); 
    sigaddset(&newmask, END_SIGNAL); 

    while(wait){
        sigsuspend(&newmask);
    }
    printf("signals in catcher: %d\n", signals_back);

    if(strcmp(mode, "kill") == 0 || strcmp(mode, "sigrt") == 0){
        for(int i = 0; i < signals_back; i++){
            kill(sender_pid, COUNT_SIGNAL);
        }
        kill(sender_pid, END_SIGNAL);
    }else if (strcmp(mode, "sigqueue") == 0){
        union sigval value;
        value.sival_int = 0;
        for (int i = 0; i < signals_back; ++i){   
            
            sigqueue(sender_pid, COUNT_SIGNAL, value);
        }
        sigqueue(sender_pid, END_SIGNAL, value);
    }

    return 0;
}
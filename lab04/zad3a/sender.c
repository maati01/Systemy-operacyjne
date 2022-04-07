#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int signals_back = 0;
int COUNT_SIGNAL = SIGUSR1;
int END_SIGNAL = SIGUSR2;
int wait = 1;
int signals_in_catcher;
int catching = 1;

void sigusr2_handle() {
    catching = 0;
}

void send_handler(int sig, siginfo_t *info, void *ucontext) {
    if (sig != COUNT_SIGNAL)
        return;
    // wait = 1;
    signals_back++;
}

void finish_handler(int sig, siginfo_t *info, void *ucontext) {
    if (sig != END_SIGNAL)
        return;
    wait = 0;
    signals_in_catcher = info->si_value.sival_int;
}

int main(int argc, char** argv){
    int catcher_pid = atoi(argv[1]);
    int n_send = atoi(argv[2]);
    char* mode = argv[3];

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

    if(strcmp(mode, "kill") == 0 || strcmp(mode, "sigrt") == 0){
        for(int i = 0; i < n_send; i++){
            kill(catcher_pid, COUNT_SIGNAL);
        }
        kill(catcher_pid, END_SIGNAL);
    }else if (strcmp(mode, "sigqueue") == 0){
        union sigval value;
        value.sival_int = 0;
        for (int i = 0; i < n_send; ++i){   
            sigqueue(catcher_pid, COUNT_SIGNAL, value);
        }
        sigqueue(catcher_pid, END_SIGNAL, value);
    }
    sigset_t newmask;
    sigemptyset(&newmask); 
    sigaddset(&newmask, END_SIGNAL); 
    while(wait){
        sigsuspend(&newmask);
    }

    printf("sender PID: %d\n", getpid());
    printf("catcher PID: %d\n", catcher_pid);
    printf("n: %d\n", n_send);
    printf("mode: %s\n", mode);
    printf("signals in sender: %d\n", signals_back);
    return 0;
}
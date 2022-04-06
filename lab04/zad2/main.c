#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>


void handle_info(int sig, siginfo_t *info, void *ucontext) {
    printf("flag: SA_SIGINFO\n");
    printf("Signal number: %d\n", info->si_signo);
    printf("PID: %d\n\n", info->si_pid);
}

void handle_stop(int sig, siginfo_t *info, void *ucontext){
    printf("Child stopped!\n");
    printf("flag: SA_NOCLDSTOP\n");
    printf("Signal number: %d\n", info->si_signo);
    printf("child PID: %d\n", info->si_pid);
    printf("Exit code %d\n\n", info->si_status);
}

void handle_onstack(int sig, siginfo_t *info, void *ucontext){
    void* p = NULL;
    printf("Process exceeded its stack limit\n");
    printf("Alternative stack address %p\n", (void*)&p);
    free(p);
    printf("flag: SA_ONSTACK\n");
    printf("Signal number: %d\n", info->si_signo);
    printf("PID: %d\n\n", info->si_pid);

    
}

void test_1(){
    struct sigaction act;
    act.sa_sigaction = handle_info;
    sigemptyset(&act.sa_mask); 
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGINT, &act, NULL) == -1){
        perror("error sigaction");
        exit(EXIT_FAILURE);
    }
    raise(SIGINT);

}
void test_2(){
    struct sigaction act;
    act.sa_sigaction = handle_stop;
    sigemptyset(&act.sa_mask); 
    act.sa_flags = SA_NOCLDSTOP;
    if(sigaction(SIGCHLD, &act, NULL) == -1){
        perror("error sigaction");
        exit(EXIT_FAILURE);
    }

    pid_t child_pid = fork();
    if (child_pid == 0) {
        exit(0);
    }
    else{
        wait(NULL);
    }
    
}
void test_3(){
    void* p = NULL;
    printf("Current stack address %p\n", (void*)&p);
    free(p);

    struct sigaction act;
    stack_t ss;

    ss.ss_sp = malloc(SIGSTKSZ);
    if (ss.ss_sp == NULL) {
        perror("error malloc");
        exit(EXIT_FAILURE);
    }

    ss.ss_size = SIGSTKSZ;
    ss.ss_flags = 0;
    if (sigaltstack(&ss, NULL) == -1) {
        perror("error sigaltstack");
        exit(EXIT_FAILURE);
    }

    act.sa_flags = SA_ONSTACK;
    act.sa_sigaction = handle_onstack;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGSEGV, &act, NULL) == -1) {
        perror("error sigaction");
        exit(EXIT_FAILURE);
    }
    raise(SIGSEGV);

}

int main(){
    test_1();
    test_2();
    test_3();
    return 0;
}

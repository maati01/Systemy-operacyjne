#include <stdio.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <signal.h>
#include <errno.h>
#include "helper.c"
#include <string.h>
#include <unistd.h>
#include <time.h>

int server_id, client_id;
struct msg client, server;
pid_t pid;
int child;

void stop(int type){
    time_t t = time(NULL);
    client.msg_text.time = *localtime(&t);
    client.msg_type = type;
    client.msg_text.id = client_id;

    strcpy(client.msg_text.buf, "CLIENT STOPPED");
    if (type == STOP) {
        if(msgsnd(server_id, &client, sizeof(client.msg_text), 0) == -1){
            error("Error while sending STOP message");
        }
    }
    kill(pid, SIGINT);
    msgctl(client_id, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}

void list(){
    time_t t = time(NULL);
    client.msg_text.time = *localtime(&t);
    client.msg_type = LIST;
    client.msg_text.id = client_id;

    strcpy(client.msg_text.buf, "LIST");
    if (msgsnd(server_id, &client, sizeof(client.msg_text), 0) == -1) {
        error("Error while sending STOP message");
    }
}


void to_all(char* text){
    time_t t = time(NULL);
    client.msg_text.time = *localtime(&t);
    client.msg_type = TO_ALL;
    client.msg_text.id = client_id;

    strcpy(client.msg_text.buf, text);
    if (msgsnd(server_id, &client, sizeof(client.msg_text), 0) == -1) {
        error("Error while sending STOP message");
    }

}

void to_one(char* text){
    time_t t = time(NULL);
    client.msg_text.time = *localtime(&t);
    client.msg_type = TO_ONE;
    client.msg_text.id = client_id;

    strcpy(client.msg_text.buf, text);
    if (msgsnd(server_id, &client, sizeof(client.msg_text), 0) == -1) {
        error("Error while sending STOP message");
    }

}

void receive_meesage(){
    while(1){
        if(msgrcv(client_id, &server, sizeof(server.msg_text), 0, 0) == -1){
            error("ERROR");
        }
        switch(server.msg_type){
            case STOP:
                printf("CLIENT STOPPED!\n");
                stop(STOP);
                break;
            case SERVER_STOP:
                printf("SERVER STOPPED!\n");
                stop(SERVER_STOP);
                break;
            case LIST:
                printf("\n-----------\nCLIENTS: \n%s-----------\n>>",server.msg_text.buf);
                fflush(stdout);
                break;
            case TO_ALL:
                printf("New meesage: %s\n", server.msg_text.buf);
                break;
            case TO_ONE:
                printf("New meesage: %s\n", server.msg_text.buf);
                break;
            case WRONG_ID:
                printf("WRONG ID!\nYOU CANNOT SEND THE MESSAGE!");
                break;
            
        }
    }
}

void send_meesage(){
    char command[256];
    char str_type[10];
    char* token;
    char* text;

    while (1) {
        strcpy(str_type, "");

        printf(">> ");
        fgets(command, 1099, stdin);
        sscanf(command, "%s", str_type);

        int type = string_to_type(str_type);

        switch (type) {
            case STOP:
                kill(child, SIGINT);
                stop(STOP);
                break;
            case LIST:
                list();
                break;
            case TO_ALL:
                strtok_r(command, " ", &text);
                to_all(text);
                break;
            case TO_ONE:
                strtok_r(command, " ", &text);
                to_one(text);
                break;
            default:
                printf("Wrong argument!\n");
                break;
        }

    }
}

void signal_handler(){
    //wysalac do servera info o zamknieciu
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    pid = getpid();
    char* homedir = getenv("HOME");

    key_t key;


    client_id = msgget(IPC_PRIVATE, QUEUE_PERMISSIONS);
    printf("[CLIENT] ID %d\n", client_id);
    key = ftok(homedir, PROJ_ID);
    printf("[CLIENT] KEY %d\n", key);

    server_id = msgget(key, 0);
    printf("[CLIENT] SERVER ID %d\n", server_id );
    client.msg_type = NEW_CLIENT;
    client.msg_text.id = client_id;
    sprintf(client.msg_text.buf, "%d", client_id);

    struct sigaction action;
    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGINT);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);

    if (msgsnd(server_id, &client, sizeof(client.msg_text), 0) != -1) {
        printf("INIT action sent to the server\n");
    }
    msgrcv(client_id, &server, sizeof(server.msg_text), NEW_CLIENT, 0);

    if ((child = fork()) == 0) {
        receive_meesage();
    }
    else {
        send_meesage();
    }

    return 0;
}
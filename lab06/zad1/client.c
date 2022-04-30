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

void stop(){
    time_t t = time(NULL);
    client.msg_text.time = *localtime(&t);
    client.msg_type = STOP;
    client.msg_text.id = client_id;
    client.msg_text.info = 0;
    strcpy(client.msg_text.buf, "CLIENT STOPPED");
    if (msgsnd(server_id, &client, sizeof(client.msg_text), 0) == -1) {
        error("Error while sending STOP message");
    }
    exit(EXIT_SUCCESS);
}

void list(){
    time_t t = time(NULL);
    client.msg_text.time = *localtime(&t);
    client.msg_type = LIST;
    client.msg_text.id = client_id;
    client.msg_text.info = 0;
    strcpy(client.msg_text.buf, "LIST");
    if (msgsnd(server_id, &client, sizeof(client.msg_text), 0) == -1) {
        error("Error while sending STOP message");
    }
    // exit(EXIT_SUCCESS);
}


void to_all(char* text){
    time_t t = time(NULL);
    client.msg_text.time = *localtime(&t);
    client.msg_type = TO_ALL;
    client.msg_text.id = client_id;
    // client.msg_text.info = 0;
    printf("WYSYLAM! \n");
    strcpy(client.msg_text.buf, text);
    if (msgsnd(server_id, &client, sizeof(client.msg_text), 0) == -1) {
        error("Error while sending STOP message");
    }else{
        printf("WYSYLANO DO: %d\n", server_id);
    }

}

void to_one(char* text, int id){
    time_t t = time(NULL);
    client.msg_text.time = *localtime(&t);
    client.msg_type = TO_ONE;
    client.msg_text.id = client_id;
    client.msg_text.info = 0;
    strcpy(client.msg_text.buf, "MEESAGE");
    if (msgsnd(server_id, &client, sizeof(client.msg_text), 0) == -1) {
        error("Error while sending STOP message");
    }

}

//zmienic qid na pid
//sprawdic czy dobrze dziala
void receive_meesage(){ //moze handler trzeba ustawic
    printf("CID: %d\n",client_id);
    while(1){
        if(msgrcv(client_id, &server, sizeof(server.msg_text), 0, 0) == -1){
            error("ERROR");
        }else{
            printf("ROMAN GIGA KOKS: %d\n",server.msg_type);
        }
        if(server.msg_type == TO_ALL){
            printf("CHUJ\n");
        }
        switch(server.msg_type){
            case TO_ALL:
                printf("New meesage: %s\n", server.msg_text.buf);
                break;
            
        }
        // sleep(1);
    }
}

void send_meesage(){
    char command[256];
    char str_type[10];
    char *token;
    char *text;

    while (1) {
        strcpy(str_type, "");

        printf(">> ");
        fgets(command, 1099, stdin);
        sscanf(command, "%s", str_type);

        // long type = string_to_mtype(str_type);

        // time_t t = time(NULL);
        // // client_request.mtext.time = *localtime(&t);
        // client.msg_type = LIST;
        // client.msg_text.id = getpid();
        // client.msg_text.info = client_id;
        // strcpy(client.msg_text.buf, command);
        // sscanf(command, " %s", str_type);
        int type = string_to_type(str_type);
        // if (msgsnd(server_id, &client, sizeof(client.msg_text), 0) == -1) {
        //     perror("Error while sending LIST message");
        // }
        switch (type) {
            case STOP:
                kill(child, SIGINT);
                stop();
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
                token = strtok_r(text, " ", &text);
                if (token != NULL && atoi(token) != 0) {
                    to_one(text, atoi(token));
                }
                else {
                    printf("[warning] Wrong 2ONE client_id!\n");
                }
                break;
            default:
                printf("Wrong argument!\n");
                break;
        }
        // sleep(1);
    }
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
    client.msg_text.id = pid;
    client.msg_text.info = client_id;
    sprintf(client.msg_text.buf, "%d", client_id);

    // send message to server

    if (msgsnd(server_id, &client, sizeof(client.msg_text), 0) != -1) {
        printf("INIT action sent to the server\n");
    }
    msgrcv(client_id, &server, sizeof(server.msg_text), NEW_CLIENT, 0);

    // client_id = server.msg_text.id;

    if ((child = fork()) == 0) {
        receive_meesage();
    }
    else {
        send_meesage();
        // receive_meesage();
    }

    return 0;
}
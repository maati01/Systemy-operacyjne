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

void stop(){
    time_t t = time(NULL);
    client.msg_text.time = *localtime(&t);
    client.msg_type = STOP;
    // client_msg.mtext.id = client_id;
    // client_msg.mtext.info = 0;
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
    // client_msg.mtext.id = client_id;
    // client_msg.mtext.info = 0;
    strcpy(client.msg_text.buf, "LIST");
    if (msgsnd(server_id, &client, sizeof(client.msg_text), 0) == -1) {
        error("Error while sending STOP message");
    }
    // exit(EXIT_SUCCESS);
}


void to_all(char* text){
    time_t t = time(NULL);
    client.msg_text.time = *localtime(&t);
    client.msg_type = STOP;
    // client_msg.mtext.id = client_id;
    // client_msg.mtext.info = 0;
    strcpy(client.msg_text.buf, text);
    if (msgsnd(server_id, &client, sizeof(client.msg_text), 0) == -1) {
        error("Error while sending STOP message");
    }

}

void to_one(char* text, int id){
    time_t t = time(NULL);
    client.msg_text.time = *localtime(&t);
    client.msg_type = STOP;
    // client_msg.mtext.id = client_id;
    // client_msg.mtext.info = 0;
    strcpy(client.msg_text.buf, "MEESAGE");
    if (msgsnd(server_id, &client, sizeof(client.msg_text), 0) == -1) {
        error("Error while sending STOP message");
    }

}


void receive_meesage(){

}

void send_meesage(){
    char str[1100];
    char str_type[1100];
    char *token;
    char *text;

    while (1) {
        strcpy(str_type, "");

        printf(">> ");
        fgets(str, 1099, stdin);
        sscanf(str, "%s", str_type);

        // long type = string_to_mtype(str_type);

        time_t t = time(NULL);
        // client_request.mtext.time = *localtime(&t);
        client.msg_type = LIST;
        client.msg_text.id = getpid();
        // client_request.mtext.info = 0;
        strcpy(client.msg_text.buf, str);
        if (msgsnd(server_id, &client, sizeof(client.msg_text), 0) == -1) {
            perror("Error while sending LIST message");
        }
        // switch (type) {
        //     case STOP:
        //         kill(child, SIGINT);
        //         stop();
        //         break;
        //     case LIST:
        //         list();
        //         break;
        //     case TO_ALL:
        //         strtok_r(str, " ", &text);
        //         to_all(text);
        //         break;
        //     case TO_ONE:
        //         strtok_r(str, " ", &text);
        //         token = strtok_r(text, " ", &text);
        //         if (token != NULL && atoi(token) != 0) {
        //             to_one(text, atoi(token));
        //         }
        //         else {
        //             printf("[warning] Wrong 2ONE client_id!\n");
        //         }
        //         break;
        //     default:
        //         printf("Wrong argument!\n");
        //         break;
        // }
        sleep(1);
    }
}

int main(int argc, char* argv[]) {
    char* homedir = getenv("HOME");

    key_t key;
    int child;
    struct msg server_response;


    client_id = msgget(IPC_PRIVATE, QUEUE_PERMISSIONS);

    key = ftok(homedir, PROJ_ID);

    server_id = msgget(key, 0);

    client.msg_type = NEW_CLIENT;
    client.msg_text.id = getpid();
    client.msg_text.info = client_id;
    sprintf(client.msg_text.buf, "%d", client_id);

    // send message to server

    if (msgsnd(server_id, &client, sizeof(struct msg_text), 0) != -1) {
        printf("INIT action sent to the server\n");
    }
    msgrcv(client_id, &server, sizeof(struct msg_text), NEW_CLIENT, 0);

    client_id = server.msg_text.id;
    printf("HERE: %ld", client_id);

    if ((child = fork()) == 0) {
        receive_meesage();
    }
    else {
        send_meesage();
    }

    return 0;
}
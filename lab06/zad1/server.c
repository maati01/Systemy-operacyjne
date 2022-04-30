#include <stdio.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <signal.h>
#include <errno.h>
#include "helper.c"
#include <string.h>

int is_server_running = 1;
int curr_usr_id = 0;
struct client_info clients[MAX_CLIENTS_NUMBER];
struct msg server, client;
int server_id;

void signal_handler(int signal_num) {
    printf("Server: Signal SIGINT received\n");
    is_server_running = 0;
    exit(0);
    // if (active_users_counter > 0) {
    //     send_shutdown_to_all_clients();
    // } else {
    //     print_sth_and_exit("Closing server!", 0);
    // }
}

void stop(){
    printf("[server] Client stopped, id: %d\n", client.msg_text.id);
    clients[client.msg_text.id].id = -1;
}

void list(){
    char buf[256] = "";
    char temp_str[256];

    for (int i = 0; i < MAX_CLIENTS_NUMBER; ++i) {
        if (clients[i].id != -1) {
            sprintf(temp_str, "%d\n", clients[i].id);
            strcat(buf, temp_str);
        }
    }
    server.msg_type = LIST;
    strcpy(server.msg_text.buf, buf);
    msgsnd(client.msg_text.id, &server, sizeof(server.msg_text), 0);
}

void to_all(){
    printf("Meesage sent to all!\n");

    server.msg_type = TO_ALL;
    strcpy(server.msg_text.buf, client.msg_text.buf);

    for (int i = 0; i < MAX_CLIENTS_NUMBER; ++i) {
        if (clients[i].id != -1 && clients[i].qid != client.msg_text.id){
            if(msgsnd(clients[i].qid, &server, sizeof(server.msg_text), 0) == -1){
                error("PRZYPS\n");
            };
        } 
        
    }
}

void to_one(){
    int target_id;
    char* command;

    target_id = atoi(strtok_r(client.msg_text.buf, " ", &command));

    server.msg_type = TO_ONE;
    strcpy(server.msg_text.buf, command);
    msgsnd(clients[target_id].qid, &server, sizeof(server.msg_text), 0);
    //uzyc wrong id
}

void new_client(){
    clients[curr_usr_id].id = curr_usr_id;
    clients[curr_usr_id].qid = client.msg_text.id;

    server.msg_type = NEW_CLIENT;
    server.msg_text.id = curr_usr_id;
    
    strcpy(server.msg_text.buf, "");
    if (msgsnd(client.msg_text.id, &server, sizeof(server.msg_text), 0) == -1) {
        error("Error while sending INIT message");
    }

    printf("[server] New client with id: %d\n", curr_usr_id);

    ++curr_usr_id;
}

void run_command(){
    switch (client.msg_type) {
        case STOP:
            printf(">> STOP\n");
            stop();
            break;
        case LIST:
            printf(">> LIST\n");
            list();
            break;
        case TO_ALL:
            printf(">> TO ALL\n");
            to_all();
            break;
        case TO_ONE:
            printf(">> TO ONE\n");
            to_one();
            break;
        case NEW_CLIENT:
            printf(">> NEW CLIENT\n");
            new_client();
            break;
        default:
            break;
    }
}

int main(int argc, char** argv){

    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        clients[i].id = -1;
    }

    char* homedir = getenv("HOME");

    key_t key = ftok(homedir, PROJ_ID);
    server_id = msgget(key, IPC_CREAT | QUEUE_PERMISSIONS);
    printf("[SERVER] KEY %d\n", key);
    printf("[SERVER] ID %d\n", server_id);

    struct sigaction action;
    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGINT);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);
    printf("Server is running!\n");

    while (is_server_running) {
        if(msgrcv(server_id, &client, sizeof(client.msg_text), 0, 0) == 1){
            printf("ERROR");
        }
        run_command();
    }
    
    msgctl(server_id, IPC_RMID, NULL);
    return 0;
}
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
    printf("Clients: \n");
    // printf("%-10s %-10s %-10s\n", "client_id", "msqid", "pid");
    for (int i = 0; i < MAX_CLIENTS_NUMBER; ++i) {
        if (clients[i].id != -1) {
            printf("id: %d\n", clients[i].id);
        }
    }
}

void to_all(){
    printf("Meesage sent to all!\n");

    // server_msg.mtext.time = client_msg.mtext.time;
    server.msg_type = TO_ALL;
    // server.msg_text.id = client.msg_text.id;
    // server.msg_text.info = 0;
    strcpy(server.msg_text.buf, client.msg_text.buf);
    printf("CURRENT : %d\n", client.msg_text.id);
    for (int i = 0; i < MAX_CLIENTS_NUMBER; ++i) {
        if (clients[i].id != -1 ){
            if(msgsnd(clients[i].pid, &server, sizeof(server.msg_text), 0) == -1){
                error("PRZYPS\n");
            };
            printf("SENT TO: %d\n", clients[i].id);
            printf("QID: %d\n", clients[i].pid);
        } 
        
    }
}

void to_one(){

}

void new_client(){
    clients[curr_usr_id].id = curr_usr_id;
    // printf("1: %d\n", clients[curr_usr_id].qid);
    // clients[i].msqid = client_msg.mtext.info;
    clients[curr_usr_id].pid = client.msg_text.info;
    printf("CLIENT PID: %d\n",clients[curr_usr_id].pid);
    server.msg_type = NEW_CLIENT;
    server.msg_text.id = curr_usr_id;
    server.msg_text.info = -1;
    
    strcpy(server.msg_text.buf, "");
    if (msgsnd(client.msg_text.info, &server, sizeof(server.msg_text), 0) == -1) {
        error("Error while sending INIT message");
    }
    printf("[server] New client with id: %d\n", curr_usr_id);

    ++curr_usr_id;
}

void run_command(){
    switch (client.msg_type) {
        case STOP:
            printf(">>> STOP\n");
            stop();
            break;
        case LIST:
            printf(">>> LIST\n");
            list();
            break;
        case TO_ALL:
            printf(">>> TO ALL\n");
            to_all();
            break;
        case TO_ONE:
            printf(">>> TO ONE\n");
            to_one();
            break;
        case NEW_CLIENT:
            printf(">>> NEW CLIENT\n");
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
        // while (msgrcv(server_id, &client, sizeof(client.msg_text), -100, IPC_NOWAIT) != -1) {
        //     run_command();
        // }
        // if (ENOMSG != errno) {  // ignore interrupting by SIGINT
        //         printf("Server: ERROR while reading input data!");
        // }        
        if(msgrcv(server_id, &client, sizeof(client.msg_text), 0, 0) != 1){
            printf("NEW COMMAND %s \n", client.msg_text.buf);
        }
        run_command();
        // if (errno != ENOMSG) perror("Error while receiving message");
        //     execute_command(&message, &response);
        

        // if (message.msg_type == STOP)
        //     continue;

        // send_message(actual_usr_id, &response, 0);
    }
    
    msgctl(server_id, IPC_RMID, NULL);
    return 0;
}
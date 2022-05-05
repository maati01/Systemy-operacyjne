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
struct client_info clients[MAX_CLIENTS_NUMBER+1];
struct msg server, client;
int server_id;
struct sigaction action;
FILE* file;

void print_time(){
    struct tm *tm = &server.msg_text.time;
    printf("[SERVER] %d-%02d-%02d %02d:%02d:%02d\n", tm->tm_year + 1900, tm->tm_mon + 1,
               tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void save_command(char* command, int client_id){
    struct tm *tm = &server.msg_text.time;
    fprintf(file, "%s - CLIENT ID: %d     %d-%02d-%02d %02d:%02d:%02d\n",command,client_id, tm->tm_year + 1900, tm->tm_mon + 1,
               tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void save_info(char* info){
    struct tm *tm = &server.msg_text.time;
    fprintf(file, "%s     %d-%02d-%02d %02d:%02d:%02d\n",info, tm->tm_year + 1900, tm->tm_mon + 1,
               tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void sending_error(int val){
    if(val == -1){
        printf("[SERVER] Sending error!\n");
        exit(EXIT_FAILURE);
    }

}

void receiving_error(int val){
    if(val == -1){
        printf("[SERVER] Receiving error!\n");
        exit(EXIT_FAILURE);
    }
}

void set_server_msg(int type, char* text){
    server.msg_type = type;
    server.msg_text.time = client.msg_text.time;
    strcpy(server.msg_text.buf, text);
}

void signal_handler(int signal_num) {
    printf("[SERVER] STOPPED!\n");
    is_server_running = 0;

    set_server_msg(SERVER_STOP, client.msg_text.buf);

    for(int i = 0; i < MAX_CLIENTS_NUMBER; i++){
        if(clients[i].id != -1){
            sending_error(msgsnd(clients[i].qid, &server, sizeof(server.msg_text), 0));
        }
        
    }
    save_info("Server is stopped!");
    fclose(file);
    msgctl(server_id, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}

void stop(){
    set_server_msg(STOP,"");
    for(int i = 0; i < MAX_CLIENTS_NUMBER; i++){
        if(client.msg_text.id == clients[i].qid){
            clients[i].id = -1;
            clients[i].qid = -1;
            break;
        }
    }

}

void list(){
    char buf[MAX_STR_SIZE] = "";
    char temp_str[MAX_STR_SIZE];

    for (int i = 0; i < MAX_CLIENTS_NUMBER; ++i) {
        if (clients[i].id != -1) {
            sprintf(temp_str, "%d\n", clients[i].id);
            strcat(buf, temp_str);
        }
    }
    set_server_msg(LIST,buf);
    sending_error(msgsnd(client.msg_text.id, &server, sizeof(server.msg_text), 0));
}

void to_all(){
    server.msg_text.sender_id = client.msg_text.sender_id;
    set_server_msg(TO_ALL,client.msg_text.buf);

    for (int i = 0; i < MAX_CLIENTS_NUMBER; ++i) {
        if (clients[i].id != -1 && clients[i].qid != client.msg_text.id){
            sending_error(msgsnd(clients[i].qid, &server, sizeof(server.msg_text), 0));
        } 
        
    }
}

void to_one(){
    int target_id;
    char* command;

    target_id = atoi(strtok_r(client.msg_text.buf, " ", &command));
    server.msg_text.sender_id = client.msg_text.sender_id;

    if(clients[target_id].id != -1){
        set_server_msg(TO_ONE,command);
        sending_error(msgsnd(clients[target_id].qid, &server, sizeof(server.msg_text), 0));
    }else{
        set_server_msg(WRONG_ID,command);
        sending_error(msgsnd(client.msg_text.id, &server, sizeof(server.msg_text), 0));
    }

    int idx = 0;
    while(clients[idx].qid != client.msg_text.id) idx++;
    save_command("2ONE", idx);
    printf("[SERVER] 2ONE - meesage sent from %d to %d!\n", idx, target_id);
}

void new_client(){
    int curr_usr_id = 0;
    while(clients[curr_usr_id].id != -1) curr_usr_id++;
    clients[curr_usr_id].id = curr_usr_id;
    clients[curr_usr_id].qid = client.msg_text.id;
    server.msg_text.id = curr_usr_id;
    
    if(curr_usr_id < MAX_CLIENTS_NUMBER){
        set_server_msg(NEW_CLIENT,"");
        printf("[SERVER] NEW CLIENT - ID: %d\n", curr_usr_id);
        save_command("NEW CLIENT", curr_usr_id);
        
    }else{
        set_server_msg(MAX_CLIENTS,"");
        printf("[SERVER] Clients limit has been reached!\n");
        save_info("Clients limit has been reached!");
    }
    sending_error(msgsnd(client.msg_text.id, &server, sizeof(server.msg_text), 0));
    
}

void run_command(){
    switch (client.msg_type) {
        case STOP:
            printf("[SERVER] STOP - CLIENT ID: %d\n", client.msg_text.sender_id);
            save_command("STOP", client.msg_text.sender_id);
            stop();
            break;
        case LIST:
            printf("[SERVER] LIST - CLIENT ID: %d\n", client.msg_text.sender_id);
            save_command("LIST", client.msg_text.sender_id);
            list();
            break;
        case TO_ALL:
            printf("[SERVER] 2ALL - CLIENT ID: %d\n", client.msg_text.sender_id);
            save_command("2ALL", client.msg_text.sender_id);
            to_all();
            break;
        case TO_ONE:
            to_one();
            break;
        case NEW_CLIENT:
            new_client();
            break;
        default:
            break;
    }
    print_time();
}

void set_sigaction(){
    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGINT);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);
}

int main(int argc, char** argv){
    for (int i = 0; i < MAX_CLIENTS_NUMBER + 1; i++) {
        clients[i].id = -1;
    }
    if((file = fopen ("./results.txt", "a+")) == NULL){
        printf("Opening file error!\n");
        exit(EXIT_FAILURE);
    }
    char* homedir = getenv("HOME");

    key_t key = ftok(homedir, PROJ_ID);
    server_id = msgget(key, IPC_CREAT | QUEUE_PERMISSIONS);

    printf("[SERVER] KEY %d\n", key);
    printf("[SERVER] ID %d\n", server_id);

    printf("[SERVER] Server is running!\n");
    
    set_sigaction();

    time_t t = time(NULL);
    server.msg_text.time = *localtime(&t);

    print_time();
    save_info("Server is running!");

    while (is_server_running) {
        receiving_error(msgrcv(server_id, &client, sizeof(client.msg_text), 0, 0));
        run_command();
    }

    fclose(file);
    msgctl(server_id, IPC_RMID, NULL);

    return 0;
}

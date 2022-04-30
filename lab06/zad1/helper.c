#ifndef HELPER_H
#define HELPER_H

#define PROJ_ID 'P'

#define QUEUE_PERMISSIONS 0660

#define MAX_CLIENTS_NUMBER 200

#define STOP 1
#define LIST 2
#define TO_ALL 3
#define TO_ONE 4
#define NEW_CLIENT 5

#include <time.h>

struct msg_text {
    int id;
    int info;
    struct tm time;
    char buf[256];
};

struct msg {
    long msg_type;
    struct msg_text msg_text;
};

struct client_info {
    int id;
    int pid;
};

int string_to_type(char *string) {
    if (strcmp(string, "STOP") == 0) {
        return STOP;
    }
    if (strcmp(string, "LIST") == 0) {
        return LIST;
    }
    if (strcmp(string, "TO_ALL") == 0) {
        return TO_ALL;
    }
    if (strcmp(string, "TO_ONE") == 0) {
        return TO_ONE;
    }
    return -1;
}
#endif  //HELPER_H
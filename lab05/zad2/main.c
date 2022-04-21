#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_emails(char* mode){
    FILE* mail;
    if(strcmp(mode,"date")==0){
        printf("\nSORTED BY DATE\n");
        mail = popen("mail -H | sort -r", "w");
    }else if(strcmp(mode,"sender")==0){
        printf("\nSORTED BY SENDER\n");
        mail = popen("mail -H | sort -k3 ", "w");
    }else{
        printf("ERROR\n");
        exit(0);    
    }
    pclose(mail);
}

void send_email(char* address, char* title, char* text){
    char command[256];
    sprintf(command, "echo %s | mail -s %s %s", text, title, address);

    FILE* mail = popen(command, "r");

    if(mail == NULL){
        printf("ERROR\n");
        exit(0);
    }
    pclose(mail);
    printf("\nEMAIL WAS SENT\n");
    printf("Address: %s\n", address);
    printf("Title: %s\n", title);
    printf("Text: %s\n", text);

}

int main(int argc, char** argv){
    if(argc != 2 && argc != 4){
        printf("ERROR\n");
        exit(0);
    }

    if(argc == 2){
        read_emails(argv[1]);
    }else{
        send_email(argv[1], argv[2], argv[3]);
    }
}
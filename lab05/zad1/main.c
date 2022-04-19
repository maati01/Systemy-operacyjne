#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

FILE* input;
int lines = 0;


int find_components_number(char* file_name){
    FILE* file = fopen(file_name, "r");
    int number = 0;
    char line[256];

    while (fgets(line, sizeof(line), file) && strcmp(line,"\n") != 0) {
        number++;
        lines++;
    }
    while (fgets(line, sizeof(line), file)) {
        lines++;
    }

    fclose(file);

    return number;

}

void read_components(char* file_name, char** components, int size){
    // int number = find_components_number(file_name);
    input = fopen(file_name, "r");
    char line[256];

    for(int i = 0; i < size; i++){
        fgets(line, sizeof(line), input);
        int j = 9;
        components[i] = calloc(256, sizeof(char));
        strcpy(components[i], &line[j+3] + 1);
    }

}

void read_commands(char** commands, int size){
    char line[256];

    fgets(line, sizeof(line), input);
    for(int i = 0; i < size; i++){
        fgets(line, sizeof(line), input);
        commands[i] = calloc(256, sizeof(char));
        strcpy(commands[i], &line[0]);
    }

    fclose(input);
}

void run_program(char** components, char** commands, int size){
    int pipes[256][2];

    for (int i = 0; i < 256; ++i) {
        if (pipe(pipes[i]) < 0) {
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < size; i++){
        int commands_counter[256];
        int size = 0;

        while (*commands[i]) { 
            if ( isdigit(*commands[i]) || ( (*commands[i]=='-'||*commands[i]=='+') && isdigit(*(commands[i]+1)) )) {
                long val = strtol(commands[i], &commands[i], 10);
                commands_counter[size] = val;
                size++;
            } else {
                commands[i]++;
            }
        }
        for(int j = 0; j < size; j++){
            pid_t pid = fork();
            if(pid < 0){
                exit(EXIT_FAILURE);
            }else if(pid == 0){
                if(j > 0){
                    dup2(pipes[j - 1][0], STDIN_FILENO);
                }
                dup2(pipes[j][1], STDOUT_FILENO);
            }
            
            for (int k = 0; k < size; k++){
                close(pipes[k][0]);
                close(pipes[k][1]);
            }
            // tutaj trzeba przygotowac argumenty 
            execvp(components[commands_counter[j]][0],components[commands_counter[j]][0]);
            exit(0);
        }
        for (int j = 0; j < size - 1; ++j){
            close(pipes[j][0]);
            close(pipes[j][1]);
        }
        for (int j = 0; j < size; ++j){
            wait(0);
        }

    }
}

int main(int argc, char** argv){
    int number = find_components_number(argv[1]);

    char** components = calloc(number, sizeof(char*));
    read_components(argv[1],components, number);
    
    number = lines - number;
    char** commands = calloc(number-1, sizeof(char*));
    read_commands(commands,number);

    // printf("%s", commands[0]); 
    // printf("%s", commands[1]); 
    // printf("%s", commands[2]); 
    // printf("%s", commands[3]); 
    run_program(components, commands, number);
    
    free(commands);
    free(components);
    return 0;
}
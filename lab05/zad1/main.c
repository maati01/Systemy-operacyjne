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

void run_line(char* line, char** components){
    int size = 0;
    char* commands_counter[256];

    while (*line) { 
        if ( isdigit(*line) || ( (*line=='-'||*line=='+') && isdigit(*(line+1)) )) {
            long val = strtol(line, &line, 10);
            commands_counter[size] = components[val-1];
            size++;
        } else {
            line++;
        }
    }

    char* args[6][3];
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 3; ++j) {
            args[i][j] = NULL;
        }
    }

    int idx = 0;
    int arg_idx = 0;
    for(int i = 0; i < size; i++){
        char* curr_command = calloc(strlen(commands_counter[i]),sizeof(char*));
        memcpy(curr_command, commands_counter[i], strlen(commands_counter[i]));
        char* token = strtok(curr_command, " ");
        while(token != NULL){
            if(strcmp(token,"|")!=0){
                args[arg_idx][idx] = token;
                idx++;
            }else{
                idx = 0;
                arg_idx++;
            }
            token = strtok(NULL, " ");
        }
        args[arg_idx][1][strlen(args[arg_idx][1])-1] = '\0';
        arg_idx++;
        idx = 0;
    }

    int pipes[6][2];

    for (int i = 0; i < arg_idx -1; ++i) {
        if (pipe(pipes[i]) < 0) {
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < arg_idx; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            if (i != arg_idx - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            if (i != 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            for (int j = 0; j < arg_idx -1 ; ++j) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            execvp(args[i][0], args[i]);
            
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < arg_idx - 1; ++i) {
        close(pipes[i][1]);
    }

    for (int i = 0; i < arg_idx; ++i) {
        wait(0);
    }
}

int main(int argc, char** argv){
    int size = find_components_number(argv[1]);

    char** components = calloc(size, sizeof(char*));
    read_components(argv[1],components, size);
    
    size = lines - size;
    char** commands = calloc(size-1, sizeof(char*));
    read_commands(commands,size);

    for(int i = 0; i < size; i++){
        run_line(commands[i], components);
    }

    free(commands);
    free(components);
    return 0;
}
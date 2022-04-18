#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

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
    for(int i = 0; i < size; i++){
        int val;
        // char* current_line = strtok(commands[i], " | ");

        while (*commands[i]) { 
            if ( isdigit(*commands[i]) || ( (*commands[i]=='-'||*commands[i]=='+') && isdigit(*(commands[i]+1)) )) {
                long val = strtol(commands[i], &commands[i], 10);
                char* current_command = strtok(commands[val-1], " | ");
                printf("%s\n", current_command); 
            } else {
                commands[i]++;
            }
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

    run_program(components, commands, number);
    
    free(commands);
    free(components);
    return 0;
}
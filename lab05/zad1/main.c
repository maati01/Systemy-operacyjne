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

char** read_components(char* file_name){
    int number = find_components_number(file_name);

    input = fopen(file_name, "r");
    char line[256];
    char** components = calloc(number, sizeof(char*));

    for(int i = 0; i < number; i++){
        fgets(line, sizeof(line), input);
        int j = 9;
        components[i] = &line[j+3] + 1;
        printf("%s", components[i]);
        // printf("%d\n", i);

    }

    // fclose(input);
    // printf("%s", components[0]);
    return components;
}

char** read_commands(int start_index){
    printf("%d\n", start_index);
    char line[256];
    int number = lines - start_index - 1;
    char** commands = calloc(number, sizeof(char*));

    for(int i = 0; i < number; i++){
        fgets(line, sizeof(line), input);
        int j = 9;
        // char index[256];

        commands[i] = &line[j+3] + 1;
        printf("%s", commands[i]);
        printf("%d\n", i);

    }

    fclose(input);

    return commands;
}

int main(int argc, char** argv){
    char** components = read_components(argv[1]);
    printf("%s", components[0]);
    free(components);

    return 0;
}
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char** blocks;
char* temporary_file_name = "temporary_file.txt";
// int reserved_index;

FILE* file;

void create_table(int size_array){
    blocks = calloc(size_array,sizeof(char*));
}

void remove_block(int index){
    free(blocks[index]);
}

int reserve_memory(char* temporary_file,int index){
    FILE* temporary_file_to_save = fopen(temporary_file, "r");
    fseek(temporary_file_to_save, 0L, SEEK_END);

    blocks[index] = calloc(ftell(temporary_file_to_save),sizeof(char));

    return index;
}

void count_lines_and_chars(int size, char** files){
    // create_table(size - 2);

    for(int i = 2; i < size; i++){
        file = fopen(files[i], "r");
        int command_length = 7 + strlen(files[i]) + strlen(temporary_file_name);
        char* command = calloc(command_length,sizeof(char)); 

        snprintf(command, command_length,"wc %s > %s", files[i], temporary_file_name);

        system(command);

        // reserved_index = reserve_memory(temporary_file_name,i-2);

        fclose(file);
        free(command);
    }    
}

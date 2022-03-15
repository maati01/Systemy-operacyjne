
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char** blocks;
char* temporary_file_name = "tmp.txt";

FILE* file;

void create_table(int size_array){
    blocks = calloc(size_array,sizeof(char*));
}

void remove_block(int index){
    free(blocks[index]);
}

void count_lines_and_chars(int size, char** files){
    for(int i = 2; i < size; i++){
        file = fopen(files[i], "r");
        char command[256];

        snprintf(command, sizeof(command),"wc %s > %s", files[i], temporary_file_name);
        system(command);

        fclose(file);
        
    }    
}

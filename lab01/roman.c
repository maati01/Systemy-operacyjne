
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

char** blocks;
char char_;
int number_of_lines = 0;
int number_of_chars = 0;
int number_of_words = 0;
int results[3];

void create_table(int size_array){
    blocks = calloc(size_array,sizeof(char*));
}

void remove_block(int index){
    free(blocks[index]);
}

void count_lines_and_chars(int size, char* files, char* temporary_file_name){
    FILE* file;

    for(int i = 0; i < size; i++){
        file = fopen(files, "r");
        char_ = getc(file);

        while (char_ != EOF){
            if (char_ == '\n'){
                number_of_lines += 1;
            }
            if (isspace(char_)){
                number_of_words += 1;
            }
            else{
                number_of_chars += 1;
            }

            char_ = getc(file);
        }

        fclose(file);
    }


    results[0] = number_of_lines;
    results[1] = number_of_chars;
    results[2] = number_of_words;


    FILE* temporary_file = fopen(temporary_file_name,"w");

    printf("%s",files);
    printf("\n");
    printf("%s",temporary_file_name);
    fprintf(temporary_file,"%d %d %d\n",results[0],results[1],results[2]);
    fprintf(temporary_file,"%d %d %d",results[0],results[1],results[2]);

    fclose(temporary_file);
    
}

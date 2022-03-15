
#include <stdio.h>
#include <ctype.h>

char** blocks;
char char_;
int number_of_lines = 0;
int number_of_chars = 0;
int number_of_words = 0;

void create_table(int size_array){
    char blocks = calloc(size_array,sizeof(char*));
}

void remove_block(int index){
    free(blocks[index]);
}

void count_lines_and_chars(int size, char files[]){
    FILE* file;

    for(int i = 0; i < size; i++){
        file = open(files[i], "r");
        char_ = getc(file);

        while (char_ != EOF){
            if (char_ = 'n'){
                number_of_lines += 1;
            }
            if (isspace(char_)){
                number_of_words += 1;
            }
            else{
                number_of_chars += 1;
            }
            
        }

        fclose(file);
    }
    
    
}

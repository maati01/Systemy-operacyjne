#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include<string.h>

FILE* file;
FILE* result;
char curr_char[1];
int exists_char_in_line = 0;
int line_length = 0;
int const char_size = sizeof(char);
char* line;

void back_pointer_and_save(FILE* input, FILE* output, int length){
    line = calloc(length, char_size);
    fseek(input, -length, 1);
    fread(line, char_size, length,input);
    fwrite(line, char_size, length,output);
    free(line);
}

int input_verification(int argc, char** argv){
    file = fopen(argv[1], "r");
    result = fopen(argv[2], "r+");

    if(file == NULL || result == NULL || argc != 3){
        printf("Wrong input!\nEnter the names of the files: \n");
        scanf("%s %s", argv[1], argv[2]);
        return 0;
    }else{
        printf("The files are correct!\n");
        return 1;
    }
}

int main(int argc, char** argv){
    while(input_verification(argc,argv) == 0);
    while(fread(curr_char, char_size, 1, file) != 0){
        if(curr_char[0] == '\n' && exists_char_in_line){
            back_pointer_and_save(file, result, line_length+1);
            line_length = 0;
            exists_char_in_line = 0;
            continue;
        }

        line_length++;

        if(isspace(curr_char[0]) == 0){
            exists_char_in_line = 1;
        }
        if(curr_char[0] == '\n'){
            line_length = 0;
        }
        

    }
    
    if(exists_char_in_line){
        back_pointer_and_save(file, result, line_length);
    }

    printf("Lines copied successfully!\n");
    fclose(file);
    fclose(result);
    return 0;
}

#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

FILE* file;
char curr_char;
char* input_char;
int number_of_lines = 0;
int const char_size = sizeof(char);
int exists_char_in_line = 0;
int result = 0;


void input_verification(int argc, char** argv){
    file = fopen(argv[2], "r");

    if(file == NULL){
        printf("File doesn't exists!\n");
        exit(0);
    }
    if(strlen(argv[1]) != char_size){
        printf("Wrong char!\n");
        exit(0);
    }
    input_char = argv[1];
}

int main(int argc, char** argv){
    input_verification(argc, argv);
    while(fread(&curr_char, char_size, 1, file) != 0){
        if(curr_char == *input_char){
            result++;
            exists_char_in_line = 1;
        }
        if(exists_char_in_line == 1 && curr_char == '\n'){
            number_of_lines++;
        }
        if(curr_char == '\n'){
            exists_char_in_line = 0;
        }
    }

    if(exists_char_in_line == 1){
        number_of_lines++;
    }
    printf("Number of chars: %d\n",result);
    printf("Number of lines: %d\n",number_of_lines);

    return 0;
}

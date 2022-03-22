#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> 


int file;
int result;
char curr_char;
int exists_char_in_line = 0;
int line_length = 0;
int const char_size = sizeof(char);
char* line;


void back_pointer_and_save(int input, int output, int length){
    line = calloc(length, char_size);
    lseek(input, -length, SEEK_CUR);
    read(input, line, length);
    write(output, line, length);
    free(line);
}

int input_verification(int argc, char** argv){
    char* input_path = calloc(strlen(argv[1]) + 3, sizeof(char));
    char* output_path = calloc(strlen(argv[2]) + 3, sizeof(char));
    snprintf(input_path, strlen(argv[1]) + 3,"./%s", argv[1]);
    snprintf(output_path, strlen(argv[2]) + 3,"./%s", argv[2]);

    file = open(input_path, O_RDONLY);
    result = open(output_path, O_WRONLY);
    free(input_path);
    free(output_path);
    if(file == -1 || result == -1 || argc != 3){
        char file1[256];
        char file2[256];
        printf("Wrong input!\nEnter the name of the file: \n");
        scanf("%256s %256s", file1, file2);
        argv[1] = file1;
        argv[2] = file2;
        return 0;
    }else{
        printf("The files are correct!\n");
        return 1;
    }
}

int main(int argc, char** argv){
    while(input_verification(argc,argv) == 0);
    while(read(file, &curr_char, char_size) == 1){
        if(curr_char == '\n' && exists_char_in_line){
            back_pointer_and_save(file, result, line_length+1);
            line_length = 0;
            exists_char_in_line = 0;
            continue;
        }

        line_length++;

        if(isspace(curr_char) == 0){
            exists_char_in_line = 1;
        }
        if(curr_char == '\n'){
            line_length = 0;
        }
    }
    
    if(exists_char_in_line){
        back_pointer_and_save(file, result, line_length);
    }

    printf("Lines copied successfully!\n");
    close(file);
    close(result);
    return 0;
}

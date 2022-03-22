#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include<string.h>

FILE* file;
FILE* result;
char curr_char[1];
int end_of_line = 0;
int exists_char_in_line = 0;
int line_length = 0;
char* line;

int main(int argc,char** argv){
    file = fopen(argv[1],"r");
    result = fopen(argv[2],"a");
    while(fread(curr_char,sizeof(char),1,file) != 0){
        if(curr_char[0] == '\n' && exists_char_in_line){
            line = calloc(line_length+1,sizeof(char));
            fseek(file,-line_length-1,1);
            fread(line, sizeof(char), line_length+1,file);
            fwrite(line,sizeof(char),line_length+1,result);
            line_length = 0;
            exists_char_in_line = 0;

            free(line);
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
        line = calloc(line_length,sizeof(char));
        fseek(file,-line_length,1);
        fread(line, sizeof(char), line_length,file);
        fwrite(line,sizeof(char),line_length,result);
        free(line);
    }

    fclose(file);
    fclose(result);
    return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>

FILE* file;
FILE* result;
char curr_char[1];
int exists_char_in_line = 0;
int line_length = 0;
int const char_size = sizeof(char);
char* line;

clock_t start_time;
clock_t end_time;

struct tms start_tms;
struct tms end_tms;
char stats_file[] = "pomiar_zad_1a.txt";
char version[] = "library version\n";


void print_and_save_stats(double real_time, double user_time, double system_time){
    char* real_time_output = calloc(21, sizeof(char));
    char* user_time_output = calloc(21, sizeof(char));
    char* sys_time_output = calloc(20, sizeof(char));

    snprintf(real_time_output, 21,"Real time: %f\n", real_time);
    snprintf(user_time_output, 21,"User time: %f\n", user_time);
    snprintf(sys_time_output, 20,"Sys time: %f\n", system_time);

    FILE *output = fopen(stats_file, "a");
    fputs(version, output);
	fputs(real_time_output, output);
	fputs(user_time_output, output);
	fputs(sys_time_output, output);
	fclose(output);

    printf("%s", version);
    printf("%s", real_time_output);
    printf("%s", user_time_output);
    printf("%s", sys_time_output);

    free(real_time_output);
    free(user_time_output);
    free(sys_time_output);
}

double calculate_time_in_second(clock_t start, clock_t end){
    return (double)(end - start)/sysconf(_SC_CLK_TCK);
}

void start_timer(){
    start_time = times(&start_tms);
}

void end_timer(){
    end_time = times(&end_tms);
    double real_time =  calculate_time_in_second(start_time, end_time);
    double user_time = calculate_time_in_second(start_tms.tms_cutime, end_tms.tms_cutime);
    double system_time = calculate_time_in_second(start_tms.tms_cstime, end_tms.tms_cstime);

    print_and_save_stats(real_time, user_time, system_time);
}

void back_pointer_and_save(FILE* input, FILE* output, int length){
    line = calloc(length, char_size);
    fseek(input, -length, 1);
    fread(line, char_size, length, input);
    fwrite(line, char_size, length, output);
    free(line);
}

int input_verification(int argc, char** argv){
    file = fopen(argv[1], "r");
    result = fopen(argv[2], "r+");
    if(file == NULL || result == NULL || argc != 3){
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
    start_timer();
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
    end_timer();
    fclose(file);
    fclose(result);
    return 0;
}

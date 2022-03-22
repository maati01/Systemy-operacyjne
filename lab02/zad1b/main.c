#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <fcntl.h> 


int file;
int result;
char curr_char;
int exists_char_in_line = 0;
int line_length = 0;
int const char_size = sizeof(char);
char* line;

clock_t start_time;
clock_t end_time;

struct tms start_tms;
struct tms end_tms;
char stats_file[] = "pomiar_zad_1b.txt";
char version[] = "system version\n";


void print_and_save_stats(double real_time, double user_time, double system_time){
    char* real_time_output = calloc(21, sizeof(char));
    char* user_time_output = calloc(21, sizeof(char));
    char* sys_time_output = calloc(20, sizeof(char));

    snprintf(real_time_output, 21, "Real time: %f\n", real_time);
    snprintf(user_time_output, 21, "User time: %f\n", user_time);
    snprintf(sys_time_output, 20, "Sys time: %f\n", system_time);

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
    start_timer();
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
    end_timer();
    close(file);
    close(result);
    return 0;
}

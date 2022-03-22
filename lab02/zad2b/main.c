#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int file;
char curr_char;
char* input_char;
int number_of_lines = 0;
int const char_size = sizeof(char);
int exists_char_in_line = 0;
int result = 0;

clock_t start_time;
clock_t end_time;

struct tms start_tms;
struct tms end_tms;

char stats_file[] = "pomiar_zad_2b.txt";
char version[] = "system version\n";


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

void input_verification(int argc, char** argv){
    file = open(argv[2], O_RDONLY);

    if(file == -1){
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
    start_timer();
    while(read(file, &curr_char, char_size) != 0){
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
    end_timer();
    return 0;
}
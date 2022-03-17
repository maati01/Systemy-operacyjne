#include "library.c"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>

clock_t start_time;
clock_t end_time;

struct tms start_tms;
struct tms end_tms;

char result_file[] = "raport2.txt";

void save_and_remove_blocks(int size){
    for(int i = 0; i < size;i++){
        reserve_memory(i);
        remove_block(i);
    }
}

int count_number_of_files(char** array,int index){
    FILE* file;
    int cnt = 0;
    while(file = fopen(array[index],"r")){
        cnt++;
        index++;
    }

    return cnt;
}

double calculate_time_in_second(clock_t start, clock_t end){
    return (double)(end - start)/sysconf(_SC_CLK_TCK);
}

void print_and_save_stats(double diff, double user_time, double system_time, char* command){
    char* real_time_output = calloc(21,sizeof(char));
    char* user_time_output = calloc(21,sizeof(char));
    char* sys_time_output = calloc(20,sizeof(char));

    snprintf(real_time_output, 21,"Real time: %f\n", diff);
    snprintf(user_time_output, 21,"User time: %f\n", user_time);
    snprintf(sys_time_output, 20,"Sys time: %f\n", system_time);

    FILE *output = fopen(result_file,"a");
    fputs(command,output);
    fputs("\n", output);
	fputs(real_time_output,output);
	fputs(user_time_output,output);
	fputs(sys_time_output,output);
    fputs("\n", output);
	fclose(output);


    printf("%s", real_time_output);
    printf("%s", user_time_output);
    printf("%s", sys_time_output);
    printf("\n");

    free(real_time_output);
    free(user_time_output);
    free(sys_time_output);
}

void start_timer(){
    start_time = times(&start_tms);
}

void end_timer(char* command){
    end_time = times(&end_tms);
    double real_time =  calculate_time_in_second(start_time, end_time);
    double user_time = calculate_time_in_second(start_tms.tms_utime, end_tms.tms_utime);
    double system_time = calculate_time_in_second(start_tms.tms_stime, end_tms.tms_stime);

    print_and_save_stats(real_time, user_time, system_time, command);

}

int main(int argc,char** argv){
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "create_table") == 0){
            start_timer();
            create_table(atoi(argv[i+1]));
            end_timer(argv[i]);

        }
        if(strcmp(argv[i], "wc_files") == 0){
            start_timer();
            count_lines_and_chars(count_number_of_files(argv,i + 1), argv + i + 1);
            end_timer(argv[i]);

        }
        if(strcmp(argv[i], "remove_block") == 0){
            start_timer();
            remove_block(atoi(argv[i+1]));
            end_timer(argv[i]);

        }
        if(strcmp(argv[i], "save_block") == 0){
            start_timer();
            reserve_memory(atoi(argv[i+1]));
            end_timer(argv[i]);
        }
        if(strcmp(argv[i], "save_and_remove_blocks") == 0){
            start_timer();
            save_and_remove_blocks(atoi(argv[i+1]));
            end_timer(argv[i]);
        }

    }
    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/times.h>


clock_t start_time;
clock_t end_time;

struct tms start_tms;
struct tms end_tms;

char result_file[] = "results2.txt";

double calculate_time_in_second(clock_t start, clock_t end){
    return (double)(end - start)/sysconf(_SC_CLK_TCK);
}

void print_and_save_stats(double diff, double user_time, double system_time, int n, double width, double result){
    char* real_time_output = calloc(21,sizeof(char));
    char* user_time_output = calloc(21,sizeof(char));
    char* sys_time_output = calloc(20,sizeof(char));
    char* input = calloc(100,sizeof(char));
    char* result_string = calloc(25,sizeof(char));

    snprintf(real_time_output, 21,"Real time: %f\n", diff);
    snprintf(user_time_output, 21,"User time: %f\n", user_time);
    snprintf(sys_time_output, 20,"Sys time: %f\n", system_time);
    snprintf(input, 100,"Processes: %d   Width: %f\n", n, width);
    snprintf(result_string, 25,"Result: %f\n", result);

    FILE *output = fopen(result_file,"a");
    fputs(input,output);
	fputs(real_time_output,output);
	fputs(user_time_output,output);
	fputs(sys_time_output,output);
    fputs(result_string,output);
    fputs("\n", output);
	fclose(output);

    printf("%s", input);
    printf("%s", real_time_output);
    printf("%s", user_time_output);
    printf("%s", sys_time_output);
    printf("%s",result_string);
    printf("\n");

    free(real_time_output);
    free(user_time_output);
    free(sys_time_output);
    free(input);
    free(result_string);
}

void start_timer(){
    start_time = times(&start_tms);
}

void end_timer(int n, double width, double result){
    end_time = times(&end_tms);
    double real_time =  calculate_time_in_second(start_time, end_time);
    double user_time = calculate_time_in_second(start_tms.tms_cutime, end_tms.tms_cutime);
    double system_time = calculate_time_in_second(start_tms.tms_cstime, end_tms.tms_cstime);

    print_and_save_stats(real_time, user_time, system_time, n, width, result);

}

double func(double x){
    return ((double)4)/((double)(x*x + 1));
}

int main(int argc, char** argv){
    char* end;
    pid_t pid;
    double rectangles, start_rectangles, temp_result;
    double result = 0;
    double width = strtod(argv[1],&end);
    int n = atoi(argv[2]);
    double area = (double)1/n;

    start_timer();

    for(int i = 0; i < n; i++) {
        pid = fork();
        if(pid < 0) {
            printf("Error");
            exit(1);
        } else if (pid == 0) {
            char* result_file = calloc(6 + i + 1,sizeof(char)); //inaczej alkowoac
            snprintf(result_file, 6 + i + 1,"w%d.txt", i+1);

            start_rectangles = ceil((double)(i)*area/(width));
            rectangles = ceil((double)(i+1)*area/(width));

            temp_result = 0;
            for(int j = (int)(start_rectangles); j < (int)rectangles;j++){
                temp_result += width*func(j*width);
            }

            FILE *output = fopen(result_file,"w+");
            fprintf(output, "%f", temp_result);
            free(result_file);
            exit(0); 
        } else  {
            wait(0);
        }
    }
    for(int i = 1; i <=n; i++){
        char* result_file = calloc(6 + i,sizeof(char)); //inaczej alkowoac
        char* value =  calloc(6 + i,sizeof(char));
        double line;
        snprintf(result_file, 6 + i,"w%d.txt", i);
        FILE* file = fopen(result_file, "r");
        fscanf(file, "%lf", &line);
        result += line;
        fclose(file);
        free(result_file);
        free(value);
    }

    end_timer(n, width, result);
    return 0;
}
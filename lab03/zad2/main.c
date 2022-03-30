#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/wait.h>


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

            FILE *output = fopen(result_file,"a");
            fprintf(output, "%f", temp_result);
            printf("Number of process: %d\n", i + 1);
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
    printf("%lf\n", result);
    return 0;
}
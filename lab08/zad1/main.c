#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

int** image;
int** negative_image;
int width, height, max_color_val, threads_number;
void* variant;

#define MAX_SIZE 512
#define MAX_PIXEL 255
#define CONST_TIME 1000000


void* compute_block(void* arg){
    struct timeval stop, start;
    gettimeofday(&start, NULL);

    int number = *((int *) arg);
    int from = (number) * ceil(width / threads_number);
    int to = (number != threads_number - 1) ? ((number + 1)* ceil(width / threads_number) - 1) : width - 1;

    for (int i = 0; i < height; i++){
        for (int j = from; j <= to; j++){
            negative_image[i][j] = MAX_PIXEL - image[i][j];
        }
    }

    gettimeofday(&stop, NULL);
    long unsigned int* time = malloc(sizeof(long unsigned int));
    *time = (stop.tv_sec - start.tv_sec) * CONST_TIME + stop.tv_usec - start.tv_usec;
    pthread_exit(time);
}

void* compute_numbers(void* arg){
    struct timeval stop, start;
    gettimeofday(&start, NULL);

    int number = *((int *) arg);
    int from = (MAX_PIXEL + 1) / threads_number * number;
    int to = (number != threads_number - 1) ? ((MAX_PIXEL + 1) / threads_number * (number + 1) ) : (MAX_PIXEL + 1);


    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            if (image[i][j] >= from && image[i][j] < to){
                negative_image[i][j] = MAX_PIXEL - image[i][j];
            }
        }
    }

    gettimeofday(&stop, NULL);
    long unsigned int* time = malloc(sizeof(long unsigned int));
    *time = (stop.tv_sec - start.tv_sec) * CONST_TIME + stop.tv_usec - start.tv_usec;
    pthread_exit(time);

}


void load_image(char* file_name){
    printf("%s\n", file_name);
    FILE* file = fopen(file_name, "r");
    char* buffer = calloc(MAX_SIZE, sizeof(char));
    
    fgets(buffer, MAX_SIZE, file);
    fgets(buffer, MAX_SIZE, file);
    sscanf(buffer, "%d %d\n", &width, &height);
    fgets(buffer, MAX_SIZE, file);
    sscanf(buffer, "%d\n", &max_color_val);


    image = calloc(height, sizeof(int*));
    for (int i = 0; i < height; i++){
        image[i] = calloc(width, sizeof(int));
    }

    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            fscanf(file, "%d", &image[i][j]);
        }
    }
    fclose(file);
}

void check_input(int argc, char** argv){
    if(argc != 5){
        printf("Wrong number of arguments!\n");
        exit(EXIT_FAILURE);
    }
    if((strcmp(argv[2], "numbers") != 0) && (strcmp(argv[2], "block") != 0)) {
        printf("Wrong method!\n");
        exit(EXIT_FAILURE);
    }else if((strcmp(argv[2], "numbers") == 0)){
        variant = compute_numbers;
    }else{
        variant = compute_block;
    }
}

void save_image_to_file(char* file_name){
    FILE* file = fopen(file_name, "w");

    if (file == NULL) {
        printf("ERROR FOPEN\n");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "P2\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "%d\n", MAX_PIXEL);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fprintf(file, "%d ", negative_image[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void free_memory(){
    for (int i = 0; i < height; i++) {
        free(image[i]);
        free(negative_image[i]);
    }
    free(image);
    free(negative_image);
}

void print_and_save_info(int threads_number, char* variant_str, FILE* times){
    printf("threads number: %d\n", threads_number);
    printf("variant: %s\n\n", variant_str);

    fprintf(times, "threads number: %d\n", threads_number);
    fprintf(times, "variant: %s\n", variant_str);
}

void print_and_save_total_time(struct timeval start, struct timeval stop, FILE* times){
    long unsigned int* time = malloc(sizeof(long unsigned int));
    *time = (stop.tv_sec - start.tv_sec) * CONST_TIME + stop.tv_usec - start.tv_usec;
    printf("time: %5lu [μs]\n", *time);
    fprintf(times, "time: %5lu [μs]\n\n", *time);
}

int main(int argc, char** argv){
    check_input(argc, argv);

    threads_number = atoi(argv[1]);
    char* variant_str = argv[2];
    char* input_file = argv[3];
    char* output_file = argv[4];
    char* path_to_times = "Times.txt";

    load_image(input_file);

    negative_image = calloc(height, sizeof(int *));
    
    for (int i = 0; i < height; i++) {
        negative_image[i] = calloc(width, sizeof(int));
    }

    pthread_t* threads = calloc(threads_number, sizeof(pthread_t));
    int* threads_num = calloc(threads_number, sizeof(int));

    struct timeval stop, start;
    gettimeofday(&start, NULL);

    for(int i = 0; i < threads_number; i++){
        threads_num[i] = i;
        pthread_create(&threads[i], NULL, variant, &threads_num[i]);
    }

    FILE* times = fopen(path_to_times, "a");

    print_and_save_info(threads_number, variant_str, times);

    for(int i = 0; i < threads_number; i++) {
        long unsigned int* time;
        pthread_join(threads[i], (void **) &time);
        printf("thread: %3d   time: %5lu [μs]\n", i, *time);
        fprintf(times, "thread: %3d   time: %5lu [μs]\n", i, *time);
    }

    gettimeofday(&stop, NULL);
    print_and_save_total_time(start, stop, times);

    save_image_to_file(output_file);
    free_memory();
    fclose(times);

    return 0;
}

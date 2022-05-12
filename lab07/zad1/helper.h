#include <time.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


#define ID 'A'
#define OVEN_ID 'O'
#define TABLE_ID 'T'

#define OVEN "oven"
#define TABLE "table"

#define OVEN_SIZE 5
#define TABLE_SIZE 5

#define ACCESS_PERMISION 0666
#define MAX_STR_SIZE 256

#define OVEN_SEM 0
#define TABLE_SEM 1
#define MAX_OVEN_SEM 2
#define MAX_TABLE_SEM 3
#define EMPTY_TABLE_SEM 4

#define PREPARATION_TIME 2
#define BAKING_TIME 5
#define DELIVERY_TIME 5
#define RETURN_TIME 5

struct oven{
    int array[OVEN_SIZE];
    int pizzas;
    int place_idx;
    int take_out_idx;
};

struct table{
    int array[TABLE_SIZE];
    int pizzas;
    int place_idx;
    int take_out_idx;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
} arg;

void error(int val, char* msg){
    if(val == -1){
        printf("%s\n", msg);
        exit(EXIT_FAILURE);
    }
}

int get_sem_id(){
    char* path = getenv("HOME");

    key_t key = ftok(path, ID);
    int sem_id;
    error(sem_id = semget(key, 5,0), "semget error");

    return sem_id;
}

int get_shm_oven_id(){
    key_t key_o = ftok(OVEN, OVEN_ID);
    int shm_oven_id;
    error(shm_oven_id = shmget(key_o, sizeof(struct oven), 0), "shmget");

    return shm_oven_id;
}

int get_shm_table_id(){
    key_t key_t = ftok(TABLE, TABLE_ID);
    int shm_table_id;
    error(shm_table_id = shmget(key_t, sizeof(struct table), 0), "shmget");

    return shm_table_id;
}
void block_sem(int sem_id, int sem_num){
    struct sembuf s = {.sem_num = sem_num, .sem_op = -1};
    error(semop(sem_id, &s, 1), "semop error");
}

void unblock_sem(int sem_id, int sem_num) {
    struct sembuf s = {.sem_num = sem_num, .sem_op = 1};
    error(semop(sem_id, &s, 1), "semop error");
}

char* get_time(){
    struct timeval time;
    gettimeofday(&time, NULL);
    int ml_sec = time.tv_usec / 1000;

    char* buffer = calloc(MAX_STR_SIZE, sizeof(char));
    strftime(buffer, MAX_STR_SIZE, "%H:%M:%S", localtime(&time.tv_sec));

    char* time_str = calloc(MAX_STR_SIZE, sizeof(char));
    sprintf(time_str, "%s:%03d", buffer, ml_sec);

    return time_str;
}

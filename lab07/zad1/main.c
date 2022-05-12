
#include <stdlib.h>
#include <unistd.h>
#include "helper.h"
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>


int shm_oven_id, shm_table_id, sem_id;


void set_oven(struct oven* oven){
    for (int i = 0; i < OVEN_SIZE; i++){
        oven->array[i] = -1;
    }
    oven->pizzas = 0;
    oven->place_idx = 0;
    oven->take_out_idx = 0;
}

void set_table(struct table* table){
    for (int i = 0; i < OVEN_SIZE; i++){
        table->array[i] = -1;
    }
    table->pizzas = 0;
    table->place_idx = 0;
    table->take_out_idx = 0;
}

void create_shm(){
    key_t key_oven, key_table;
    
    error(key_oven = ftok(OVEN, OVEN_ID), "ftok error");
    error(key_table = ftok(TABLE, TABLE_ID), "ftok error");

    error(shm_oven_id = shmget(key_oven, sizeof(struct oven), IPC_CREAT | ACCESS_PERMISION), "shmget error");

    error(shm_table_id = shmget(key_table, sizeof(struct table), IPC_CREAT | ACCESS_PERMISION), "shmget error");

    struct oven* oven = shmat(shm_oven_id, NULL, 0);
    struct table* table = shmat(shm_table_id, NULL, 0);

    set_oven(oven);
    set_table(table);
}

void create_sem(){
    key_t key;
    char* path = getenv("HOME");

    error(key = ftok(path, ID), "ftok error");
    error(sem_id = semget(key, 5, ACCESS_PERMISION | IPC_CREAT), "semget error");

    union arg;
    arg.val = 1;

    error(semctl(sem_id, OVEN_SEM, SETVAL, arg), "semctl error");
    error(semctl(sem_id, TABLE_SEM, SETVAL, arg), "semctl error");

    arg.val = OVEN_SIZE;
    error(semctl(sem_id, MAX_OVEN_SEM, SETVAL, arg), "semctl error");

    arg.val = TABLE_SIZE;
    error(semctl(sem_id, MAX_TABLE_SEM, SETVAL, arg), "semctl error");

    arg.val = 0;
    error(semctl(sem_id, EMPTY_TABLE_SEM, SETVAL, arg), "semctl error");

}

void run_exec(int range, char* path){
    for (int i = 0; i < range; i++){
        pid_t pid = fork();
        if (pid == 0){
            execl(path, path, NULL);
        }
    }    
}

int main(int argc, char** argv){
    char* cook_path = "./cook";
    char* supplier_path = "./supplier";

    int cooks = atoi(argv[1]);
    int suppliers = atoi(argv[2]);


    create_shm();
    create_sem();

    run_exec(cooks, cook_path);
    run_exec(suppliers, supplier_path);

    for(int i = 0; i < cooks + suppliers; i++)
        wait(NULL);

    return 0;
}

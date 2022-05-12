#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#include "helper.h"

int is_running = 1;

void place_in_oven(struct oven* oven, int type){
    oven->array[oven->place_idx] = type;
    oven->place_idx++;
    oven->place_idx = oven->place_idx % OVEN_SIZE;
    oven->pizzas++;
}

int take_out_pizza(struct oven* oven){
    int type = oven->array[oven->take_out_idx];
    oven->array[oven->take_out_idx] = -1;
    oven->take_out_idx++;
    oven->take_out_idx = oven->take_out_idx % OVEN_SIZE;
    oven->pizzas--;
    return type;
}


void place_on_table(struct table* table, int type){
    table->array[table->place_idx] = type;
    table->place_idx++;
    table->place_idx = table->place_idx % OVEN_SIZE;
    table->pizzas++;
}

int main(){

    int sem_id = get_sem_id();
    int shm_oven_id = get_shm_oven_id();
    int shm_table_id = get_shm_table_id();

    struct oven* oven = shmat(shm_oven_id, NULL, 0);
    struct table* table = shmat(shm_table_id, NULL, 0);


    int type;

    srand(getpid());
    while(is_running){
        type = rand() % 10;

        printf("COOK pid: %d timestamp: %s Przygotowuje pizze: %d\n", getpid(), get_time(), type);
        sleep(PREPARATION_TIME);

        block_sem(sem_id, MAX_OVEN_SEM);
        block_sem(sem_id, OVEN_SEM);

        place_in_oven(oven, type);
        printf("COOK pid: %d timestamp: %s Dodałem pizze: %d. Liczba pizz w piecu: %d\n",
         getpid(), get_time(), type, oven->pizzas);

        unblock_sem(sem_id, OVEN_SEM);

        sleep(BAKING_TIME);

        block_sem(sem_id, OVEN_SEM);
        type = take_out_pizza(oven);

        printf("COOK pid: %d timestamp: %s Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n",
         getpid(), get_time(), type, oven->pizzas, table->pizzas);
        unblock_sem(sem_id, OVEN_SEM);

        unblock_sem(sem_id, MAX_OVEN_SEM);
        block_sem(sem_id, MAX_TABLE_SEM);

        block_sem(sem_id, TABLE_SEM);
        place_on_table(table, type);
        printf("COOK pid: %d timestamp: %s Umieszczam pizze na stole: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n",
         getpid(), get_time(), type, oven->pizzas, table->pizzas);

        unblock_sem(sem_id, TABLE_SEM);
        unblock_sem(sem_id, EMPTY_TABLE_SEM);

    }
    return 0;

}

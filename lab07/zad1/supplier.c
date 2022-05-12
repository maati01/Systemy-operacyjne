#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#include "helper.h"

int is_running = 1;


int take_pizza(struct table* table, int sem_id){
    int type = table->array[table->take_out_idx];
    table->array[table->take_out_idx] = -1;
    table->take_out_idx++;
    table->take_out_idx = table->take_out_idx % TABLE_SIZE;
    table->pizzas--;

    printf("pid: %d timestamp: %s Pobieram pizze: %d. Liczba pizz na stole: %d\n",
     getpid(), get_time(), type, table->pizzas);
        
    unblock_sem(sem_id, TABLE_SEM);
    unblock_sem(sem_id, MAX_TABLE_SEM);

    return type;
}

int main(){

    int sem_id = get_sem_id();
    int shm_table_id = get_shm_table_id();

    struct table* table = shmat(shm_table_id, NULL, 0);

    int type;

    while(is_running) {
        block_sem(sem_id, EMPTY_TABLE_SEM);
        block_sem(sem_id, TABLE_SEM);

        type = take_pizza(table, sem_id);

        sleep(DELIVERY_TIME);
        printf("pid: %d timestamp: %s Dostarczam pizze: %d\n", getpid(), get_time(), type);
        sleep(RETURN_TIME);
    }
}

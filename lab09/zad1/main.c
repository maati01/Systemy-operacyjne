#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define ELVES_SIZE 10
#define MAX_ELVES_IN_QUEUE 3
#define REINDEERS_SIZE 9
#define MAX_GIFTS 3

pthread_mutex_t mutex_santa = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_santa   = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_reindeer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_reindeer_wait = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_reindeer   = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_elf = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_elf_wait = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_elf   = PTHREAD_COND_INITIALIZER;

int is_running = 1;
int reindeer_is_waiting = 0;
int elves_queue[MAX_ELVES_IN_QUEUE];
int elves_in_queue = 0;
int reindeers_in_queue = 0;
int deliveries_cnt = 0;

int get_random_time(int from, int to){
    return rand()%(to - from + 1) + from;
}

void reset_queue(int* queue, int size){
    for (int i = 0; i < size; i++){
        queue[i] = -1;
    }
}

void wait_if_max_elves_queue(int id){
    while (elves_in_queue == MAX_ELVES_IN_QUEUE) {
        printf("Elf: czeka na powrót elfów, %d\n", id);
        pthread_cond_wait(&cond_elf, &mutex_elf_wait);
    }
}

void solve_elves_problems(int id){
    if (elves_in_queue == MAX_ELVES_IN_QUEUE){
        printf("Elf: wybudzam Mikołaja, %d\n", id);
        pthread_mutex_lock(&mutex_santa);
        pthread_cond_broadcast(&cond_santa);
        pthread_mutex_unlock(&mutex_santa);
        printf("Elf: Mikołaj rozwiązuje problem, %d\n", id);
    }
}

void* elf_thread_func(void* arg){
    int id = *((int *) arg);

    while(is_running){
        sleep(get_random_time(2, 5));
        pthread_mutex_lock(&mutex_elf_wait);
        wait_if_max_elves_queue(id);

        pthread_mutex_unlock(&mutex_elf_wait);
        pthread_mutex_lock(&mutex_elf);

        if (elves_in_queue < MAX_ELVES_IN_QUEUE){
            elves_queue[elves_in_queue] = id;
            elves_in_queue++;
            printf("Elf: czeka %d elfów na Mikołaja, %d\n", elves_in_queue, id);
            solve_elves_problems(id);
        }
        pthread_mutex_unlock(&mutex_elf);
    }
    exit(0);
}

void check_reindeers_in_queue(){
    if (reindeers_in_queue == REINDEERS_SIZE) {
        deliveries_cnt++;

        printf("Mikołaj: dostarczam zabawki\n");
        reindeers_in_queue = 0;

        sleep(get_random_time(2, 4));

        pthread_mutex_lock(&mutex_reindeer_wait);
        reindeer_is_waiting = 0;
        pthread_cond_broadcast(&cond_reindeer);
        pthread_mutex_unlock(&mutex_reindeer_wait);
    }
}

void check_elves_in_queue(){
    if (elves_in_queue == ELVES_SIZE) {
        printf("Mikołaj: rozwiązuje problemy elfów %d %d %d\n", elves_queue[0], elves_queue[1], elves_queue[2]);
        sleep(get_random_time(1, 2));

        pthread_mutex_lock(&mutex_elf_wait);
        reset_queue(elves_queue, ELVES_SIZE);
        elves_in_queue = 0;
        pthread_cond_broadcast(&cond_elf);
        pthread_mutex_unlock(&mutex_elf_wait);
    }
}

void* santa_thread_func(void* arg){
    while (is_running){

        pthread_mutex_lock(&mutex_santa);

        while (elves_in_queue < ELVES_SIZE && reindeers_in_queue < REINDEERS_SIZE){
            pthread_cond_wait(&cond_santa, &mutex_santa);
        }

        pthread_mutex_unlock(&mutex_santa);
        printf("Mikołaj: budzę się\n");
        pthread_mutex_lock(&mutex_reindeer);

        check_reindeers_in_queue();

        pthread_mutex_unlock(&mutex_reindeer);

        if (deliveries_cnt == MAX_GIFTS){
            printf("\nMikołaj dostarczył %d prezenty!\n", MAX_GIFTS);
            break;
        }

        pthread_mutex_lock(&mutex_elf);

        check_elves_in_queue();
        
        printf("Mikołaj: zasypiam\n\n");
        pthread_mutex_unlock(&mutex_elf);
    }

    exit(0);
}

void wake_up_santa_by_reindeer(int id){
    if (reindeers_in_queue == REINDEERS_SIZE) {
        printf("Renifer: wybudzam Mikołaja, %d\n", id);
        pthread_mutex_lock(&mutex_santa);
        pthread_cond_broadcast(&cond_santa);
        pthread_mutex_unlock(&mutex_santa);
    }
}

void* reindeer_thread_func(void* arg){
    int id = *((int *) arg);

    while(is_running){
        pthread_mutex_lock(&mutex_reindeer_wait);
        while (reindeer_is_waiting) {
            pthread_cond_wait(&cond_reindeer, &mutex_reindeer_wait);
        }
        pthread_mutex_unlock(&mutex_reindeer_wait);

        sleep(get_random_time(5, 10));

        pthread_mutex_lock(&mutex_reindeer);
        reindeers_in_queue++;
        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", reindeers_in_queue, id);
        reindeer_is_waiting = 1;

        wake_up_santa_by_reindeer(id);
        pthread_mutex_unlock(&mutex_reindeer);
    }
    exit(0);
}

void create_threads(int size, void* func, int* array_id, pthread_t* threads_array){ 
    for (int i = 0; i < size; i++){
        array_id[i] = i;
        pthread_create(&threads_array[i], NULL, func, &array_id[i]);
    }
}

void join_threads(int size, pthread_t* threads_array){
    for (int i = 0; i < size; i++){
        pthread_join(threads_array[i], NULL);
    }
}

int main(int argc, char** argv){
    pthread_t santa_thread;
    pthread_create(&santa_thread, NULL, &santa_thread_func, NULL);

    int* reindeers_id_array = malloc(REINDEERS_SIZE*sizeof(int));
    pthread_t* reindeers_threads = malloc(REINDEERS_SIZE*sizeof(pthread_t));
    create_threads(REINDEERS_SIZE, reindeer_thread_func, reindeers_id_array, reindeers_threads);

    int* elves_id_array = malloc(ELVES_SIZE*sizeof(int));
    pthread_t* elves_threads = malloc(ELVES_SIZE*sizeof(pthread_t));
    create_threads(ELVES_SIZE, elf_thread_func, elves_id_array, elves_threads);

    pthread_join(santa_thread, NULL);
    join_threads(REINDEERS_SIZE, reindeers_threads);
    join_threads(ELVES_SIZE, elves_threads);

    free(reindeers_id_array);
    free(reindeers_threads);
    free(elves_id_array);
    free(elves_threads);

    return 0;
}

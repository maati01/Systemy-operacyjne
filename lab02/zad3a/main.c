#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int file_cnt = 0;
int dir_cnt = -1;
int char_dev_cnt = 0;
int block_dev_cnt = 0;
int fifo_cnt = 0;
int slink_cnt = 0;
int socket_cnt = 0;

struct stat stats;

void convert_and_print_time() {
    char* date = calloc(20, sizeof(char));
    strftime(date, 20, "%Y-%m-%d %H:%M:%S", localtime(&stats.st_atime));
    printf("Date of last access: %s\n", date);

    strftime(date, 20, "%Y-%m-%d %H:%M:%S", localtime(&stats.st_mtime));
    printf("Date of last modification: %s\n", date);
    free(date);
}

void file_stats(char* file_path, int type, char* file_name){
    stat(file_path, &stats);

    printf("File name: %s\n", file_name);
    
    switch (type)
    {
    case DT_BLK:
        block_dev_cnt++;
        printf("Type name: block device\n");
        break;
    case DT_CHR:
        char_dev_cnt++;
        printf("Type name: character device\n");
        break;
    case DT_DIR:
        dir_cnt++;
        printf("Type name: directory\n");
        break;
    case DT_FIFO:
        fifo_cnt++;
        printf("Type name: FIFO\n");
        break;
    case DT_LNK:
        slink_cnt++;
        printf("Type name: symbolic link\n");
        break;
    case DT_REG:
        file_cnt++;
        printf("Type name: regular file\n");
        break;
    case DT_SOCK:
        socket_cnt++;
        printf("Type name: socket\n");
        break;
    default:
        break;
    }
    printf("Path: %s\n", file_path);
    printf("Number of links: %ld\n", stats.st_nlink);
    printf("Size: %ld\n", stats.st_size);
    convert_and_print_time();
    printf("\n");
}

void iterate_through_dir(char* dir_path, char* file_name){
    DIR* dir = opendir(dir_path);

    if(dir == NULL){
        return;
    }

    struct dirent *entry;
    entry = readdir(dir);
    file_stats(dir_path, entry->d_type, file_name);
    while (entry != NULL){   
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
            char* curr_path = calloc(strlen(dir_path) + strlen(entry->d_name) + 1, sizeof(char));
            strcpy(curr_path, dir_path);
            strcat(curr_path, "/");
            strcat(curr_path, entry->d_name);
            if(entry->d_type == DT_DIR){
                iterate_through_dir(curr_path, entry->d_name);
            }else{
                file_stats(curr_path, entry->d_type, entry->d_name);
            }

            free(curr_path);
        }
        entry = readdir(dir);
    }
    closedir(dir);
}

void print_counters(){
    printf("Number of \n");
    printf("block devices: %d\n", block_dev_cnt);
    printf("character devices: %d\n", char_dev_cnt);
    printf("directories: %d\n", dir_cnt);
    printf("FIFOs: %d\n", fifo_cnt);
    printf("symbolic links: %d\n", slink_cnt);
    printf("regular files: %d\n", file_cnt);
    printf("sockets: %d\n", socket_cnt);  
}
int main(int argc, char** argv){
    char* path = realpath(argv[1], NULL);
    iterate_through_dir(path, argv[1]);
    print_counters();

    return 0;
}

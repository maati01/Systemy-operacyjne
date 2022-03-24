#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ftw.h>

#define _XOPEN_SOURCE 500

int file_cnt = 0;
int dir_cnt = -1;
int char_dev_cnt = 0;
int block_dev_cnt = 0;
int fifo_cnt = 0;
int slink_cnt = 0;
int socket_cnt = 0;

         
void convert_and_print_time(long int atime, long int mtime) {
    char* date = calloc(20, sizeof(char));
    strftime(date, 20, "%Y-%m-%d %H:%M:%S", localtime(&atime));
    printf("Date of last access: %s\n", date);
    
    strftime(date, 20, "%Y-%m-%d %H:%M:%S", localtime(&atime));
    printf("Date of last modification: %s\n", date);
    free(date);
}

int file_stats (const char* file_path,const struct stat* stats, int flag, struct FTW* ftw_struct){

    switch (flag)
    {
    case FTW_D:
        dir_cnt++;
        printf("Type name: directory\n");
        break;
    case FTW_SL:
        slink_cnt++;
        printf("Type name: symbolic link\n");
        break;
    case FTW_F:
        file_cnt++;
        printf("Type name: regular file\n");
        break;
    }
    switch (stats->st_mode & S_IFMT)
    {
    case S_IFBLK:
        block_dev_cnt++;
        printf("Type name: block device\n");
        break;
    
    case S_IFCHR:
        char_dev_cnt++;
        printf("Type name: character device\n");
        break;
    case S_IFIFO:
        fifo_cnt++;
        printf("Type name: FIFO\n");
        break;
    case S_IFSOCK:
        socket_cnt++;
        printf("Type name: socket\n");
        break;
    }

    printf("Path: %s\n", file_path);
    printf("Number of links: %ld\n", stats->st_nlink);
    printf("Size: %ld\n", stats->st_size);
    convert_and_print_time(stats->st_atime, stats->st_mtime);
    printf("\n");
    
    return 0;
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
    int fd_limit = 5;

    nftw(path, file_stats, fd_limit, FTW_D | FTW_F| FTW_SL);
    print_counters();

    return 0;
}

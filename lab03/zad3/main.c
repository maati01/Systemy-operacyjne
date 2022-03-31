#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

FILE* file;
char* start_dir_path;

char* str_remove(char *str, const char *sub) {
    size_t len = strlen(sub);
    if (len > 0) {
        char *p = str;
        while ((p = strstr(p, sub)) != NULL) {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    return ++str;
}

void find_substring(char* expected, char* file_path, int PID){
    file = fopen(file_path, "r");
    
    char *line = NULL;
    size_t len = 0;

    while(getline(&line, &len, file) != -1) {

        if (strstr(line,expected) != NULL){
            printf("path: %s\n", str_remove(file_path,start_dir_path));
            printf("PID: %d\n\n", PID);
        }
    }
    fclose(file);
    free(line);

}

void iterate_through_dir(char* dir_path, char* file_name, char* expected, int depth, int max_depth, int PID){
    DIR* dir = opendir(dir_path);
    pid_t pid;

    if(dir == NULL){
        return;
    }

    struct dirent *entry;
    entry = readdir(dir);
    while (entry != NULL){   
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
            char* curr_path = calloc(strlen(dir_path) + strlen(entry->d_name) + 1, sizeof(char));
            strcpy(curr_path, dir_path);
            strcat(curr_path, "/");
            strcat(curr_path, entry->d_name);
            if(entry->d_type == DT_DIR){
                pid = fork();
                
                if(pid < 0) {
                    printf("Error");
                    exit(1);
                } else if (pid == 0) {
                    depth += 1;
                    if(depth <= max_depth){
                        iterate_through_dir(curr_path, entry->d_name, expected, depth,max_depth, getpid());
                    }
                    exit(0); 
                    
                } else  {
                    wait(0);
                }
                
            }else if(strstr(entry->d_name, ".txt") != NULL){
                find_substring(expected,curr_path, PID);
                
            }

            free(curr_path);
        }
        entry = readdir(dir);
    }
    closedir(dir);
}


int main(int argc, char** argv){
    int depth = 0;
    char* path = realpath(argv[1], NULL);
    start_dir_path = path;
    iterate_through_dir(path, argv[1], argv[2], depth, atoi(argv[3]), 0);

    return 0;
}
#ifndef lab01_H
#define lab01_H
#include <stdio.h>

void create_table(int size_array);
void remove_block(int index);
int reserve_memory(char* temporary_file,int index);
void count_lines_and_chars(int size, char** files);

#endif //lab01
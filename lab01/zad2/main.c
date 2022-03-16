#include "library.c"

int main(int argc,char** argv){
    printf("%d\n",argc);
    printf("%s\n",argv[0]);
    printf("%s\n",argv[1]);
    printf("%s\n",argv[2]);
    count_lines_and_chars(argc,argv);
    return 0;
}
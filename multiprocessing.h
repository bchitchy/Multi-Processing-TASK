#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <malloc.h>
#include <sys/stat.h>
#include <ctype.h>
#include <signal.h>

#define LINE_SIZE 256
#define MAP_SIZE 4096

typedef struct
{
    char* file;
    char* out_dir;
    char* task;
    char new[50];
    char old[50];
    int upper;

}task_node;

char** parse_line(char* line);
void replace(const char *old_word, const char *new_word, const char *filename , const char*);
void convert_case(const char *filename, int to_upper , const char*);
void count_words(const char *filename , const char*);
void signal_handler(int signal);


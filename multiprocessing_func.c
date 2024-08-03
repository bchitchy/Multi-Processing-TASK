#include "multiprocessing.h"


char** parse_line(char* line) {
    char** parse = (char**)calloc(LINE_SIZE, sizeof(char*));
    if (!parse) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    int index = 0;
    char* token = strtok(line, " ");
    while (token != NULL) {
        parse[index++] = strdup(token);
        if (!parse[index-1]) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
        token = strtok(NULL, " ");
    }
    parse[index] = NULL;

    parse = realloc(parse, (index + 1) * sizeof(char*));
    if (!parse) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }
    return parse;
}


void count_words(const char *filename, const char *output_dir) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int word_count = 0;
    int in_word = 0;
    char c;

    while ((c = fgetc(file)) != EOF) {
        if (isspace(c)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            word_count++;
        }
    }

    fclose(file);

    char out_filename[FILENAME_MAX];
    snprintf(out_filename, sizeof(out_filename), "%s/word_count_%s", output_dir, filename);

    FILE *out_file = fopen(out_filename, "w");
    if (out_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fprintf(out_file, "Word count: %d\n", word_count);
    fclose(out_file);
}



void replace(const char *old_word, const char *new_word, const char *filename, const char *output_dir) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening source file");
        return;
    }

    char out_filename[FILENAME_MAX];
    snprintf(out_filename, sizeof(out_filename), "%s/replaced_%s", output_dir, filename);
    
    FILE *out_file = fopen(out_filename, "w");
    if (!out_file) {
        perror("Error opening destination file");
        fclose(file);
        return;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
        char *temp = (char*)malloc(len * 2); 
        if (!temp) {
            perror("Memory allocation failed");
            break;
        }
        temp[0] = '\0'; 

        char *remaining = line;
        char *pos;
        while ((pos = strstr(remaining, old_word)) != NULL) {
            size_t count = pos - remaining;
            strncat(temp, remaining, count); 
            strcat(temp, new_word);          
            remaining = pos + strlen(old_word); 
        }
        strcat(temp, remaining); 

        fprintf(out_file, "%s", temp);
        free(temp); 
    }

    free(line); 
    fclose(file);
    fclose(out_file);
}

void convert_case(const char *filename, int to_upper, const char *output_dir) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char out_filename[FILENAME_MAX];
    snprintf(out_filename, sizeof(out_filename), "%s/converted_%s", output_dir, filename);

    FILE *out_file = fopen(out_filename, "w");
    if (out_file == NULL) {
        perror("fopen");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    char c;
    while ((c = fgetc(file)) != EOF) {
        if (to_upper) {
            fputc(toupper(c), out_file);
        } else {
            fputc(tolower(c), out_file);
        }
    }

    fclose(file);
    fclose(out_file);
}

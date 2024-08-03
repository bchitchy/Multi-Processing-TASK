#include "multiprocessing.h"


char* tasks[] = {"count words", "replace", "convert case", NULL};

int main() {
    task_node* proc_task;

    int core_counts = sysconf(_SC_NPROCESSORS_CONF);
    if (core_counts == -1) {
        perror("sysconf");
        exit(EXIT_FAILURE);
    }

    const char file_inputs[] = "Enter file inputs: ";
    const char output_dir[] = "Enter output directory: ";
    const char process_input[] = "Enter number of processes: ";
    const char task_input[] = "Enter processing task -> (count words, replace, convert case): ";

    
    char* file_buf = NULL;
    char* out_dir_buf = NULL;
    char* task_buf = NULL;
    int process_count;

    char** parse;
    int file_count;
    
    while (1) {
        printf("%s", file_inputs);

        size_t file_buf_size = 0;
        if (getline(&file_buf, &file_buf_size, stdin) == -1) {
            perror("getline");
            exit(EXIT_FAILURE);
        }
        file_buf[strcspn(file_buf, "\n")] = '\0';

        parse = parse_line(file_buf);
        int index = 0;
        
        int check_stat;
        file_count = 0; 
        while (parse[index] != NULL) {
            struct stat file_stat;
            check_stat = stat(parse[index], &file_stat);
            if (check_stat < 0) {
                printf("Invalid file name: %s\n", parse[index]);
                break;
            }
            file_count++;
            index++;
        }

        if (check_stat == 0) {
            break;
        }
    }

    printf("%s", output_dir);

    size_t dir_size = 0;
    if (getline(&out_dir_buf, &dir_size, stdin) == -1) {
        perror("getline");
        exit(EXIT_FAILURE);
    }
    out_dir_buf[strcspn(out_dir_buf, "\n")] = 0;

    struct stat st = {0};
    if (stat(out_dir_buf, &st) == -1) {
        if (mkdir(out_dir_buf, 0700) != 0) {
            perror("mkdir");
            exit(EXIT_FAILURE);
        }
    }

    while (1) {
        printf("%s", process_input);
        if (scanf("%d", &process_count) != 1) {
            fprintf(stderr, "Invalid input. Please enter an integer.\n");
            while (getchar() != '\n'); 
            continue;
        }
        if (process_count > 0 && process_count <= core_counts) {
            break;
        } else {
            printf("Invalid count of processes\n");
        }
    }
    while (getchar() != '\n');


    while (1) {
        printf("%s", task_input);

        size_t task_size = 0;
        if (getline(&task_buf, &task_size, stdin) == -1) {
            perror("getline");
            exit(EXIT_FAILURE);
        }
        task_buf[strcspn(task_buf, "\n")] = '\0';

        int index = 0;
        while (tasks[index] != NULL && strcmp(tasks[index], task_buf) != 0) {
            index++;
        }

        if (tasks[index] == NULL) {
            printf("Undefined task name, try again\n");
        } else {
            break;
        }
    }

    char old[50] = {0};
    char new[50] = {0};
    int upper = 0;
    if (strcmp(task_buf, "replace") == 0) {   
        printf("Input new string -> ");
        scanf("%s", new);

        printf("Input old string -> ");
        scanf("%s", old);
    }

    if (strcmp(task_buf, "convert case") == 0) {   
        printf("If upper input 1, else input 0 -> ");
        scanf("%d", &upper);
    }
    
    printf("It's okay, processes started their work\n");

    proc_task = malloc(file_count * sizeof(task_node));
    if (!proc_task) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < file_count; i++) {
        proc_task[i].file = parse[i];
        proc_task[i].out_dir = out_dir_buf;
        proc_task[i].task = task_buf;
        strcpy(proc_task[i].new, new);
        strcpy(proc_task[i].old, old);
        proc_task[i].upper = upper;
    }
        
        int pipefd[2];

        if (pipe(pipefd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        write(pipefd[1] , proc_task , sizeof(task_node)*file_count);
        close(pipefd[1]);

        int i;
        for ( i = 0; i < process_count; i++) {
            pid_t pid = fork();
            if (pid == 0) {

                task_node child_task;
                int count = 0;
                int check_read;

            while ((check_read = read(pipefd[0], &child_task, sizeof(task_node)))) {

                count++;
                int index = 0;

                while (tasks[index] != NULL && strcmp(tasks[index], child_task.task) != 0) {
                    index++;
                }
                switch (index) {
                    case 0:
                        count_words(child_task.file, child_task.out_dir);
                        break;
                    case 1:
                        replace(child_task.old, child_task.new, child_task.file, child_task.out_dir);
                        break;
                    case 2:
                        convert_case(child_task.file, child_task.upper, child_task.out_dir);
                        break;
                    default:
                        break;
                }
            }   
                printf("Process %d processed %d files.\n", i + 1, count);
                close(pipefd[0]);
                exit(EXIT_SUCCESS);
        } 
        else if (pid > 0);
            
         else {
            perror("fork");
            exit(EXIT_FAILURE);
        }


    }
    for (int i = 0; i < process_count; i++)
    {
        wait(NULL);
    }
    free(proc_task);
    free(file_buf);
    free(out_dir_buf);
    free(task_buf);
    return 0;
}


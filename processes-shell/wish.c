#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

char* PATH = NULL;
size_t PATH_LEN = 0;

char* process_input(FILE* f_source);
int init_path_variable(char** PATH);
int check_executable_path(char* executable_path, char* PATH);


int check_executable_path(char* executable_path, char* PATH) {
    const char* delimiter = ":";
    // char* each_path;
    // while ((each_path = strsep(&input, delimiter)) != NULL) {
    //         printf("inputs: %s\n", token);
    //     }
    // if (access(executable_path, X_OK) == -1) {
    //     return -1;
    // }    

}

int main(int argc, char** argv) {
    if (argc > 2) {
        printf("ERROR: Bad usage.\n");
        exit(1);
    }
    int err = init_path_variable(&PATH);
    if (err == -1) {
        printf("ERROR: PATH initionalization failed.\n");
        exit(1);
    }
    
    if (argc == 2) {
        // batch mode
        FILE* fp = fopen(argv[1], "r");
        // read each lines in the file and execute commands
        // reading the line is the same as in the interactive mode.
        fclose(fp);
    }
    
    while(true) {
        printf("wish> ");
        char* input = process_input(stdin);
        if (input == NULL) {
            printf("ERROR: couldn't read the input.\n");
        }
        const char* delimiter = " ";
        char* token;
        token = strsep(&input, delimiter);
        if (token == NULL) {
            printf("ERROR: no name of an executable file.\n");
            exit(1);
        }
        // loop to check if this executable is okay within the known PATH
        if (check_executable_path() == -1) {
            printf("ERROR: cannot access the executable.\n");
            exit(1);
        }
        
        
        while ((token = strsep(&input, delimiter)) != NULL) {
            printf("inputs: %s\n", token);
        }

        free(input);
    }
}

char* process_input(FILE* f_source) {
    char* input = NULL;
    size_t len = 0;
    ssize_t nread;
    nread = getline(&input, &len, f_source);
    if (nread == -1 && feof(stdin)) {
        printf("EOF: end of stdin has reached.\n");
        exit(0);
    } else if (nread == -1 && ferror(stdin)) {
        printf("ERROR: file error from stdin.\n");
        return NULL;
    }
    // printf("%s", input);
    // immutability is important
    // remove newline character in the input
    if (input[nread - 1] == '\n') {
        input[nread - 1] = '\0';
    }
    char* input_copy = strdup(input);
    return input_copy;
}

int init_path_variable(char** PATH) {
    char* initial_path = "/bin:";
    *PATH = (char*) malloc(strlen(initial_path) + 1);
    if (*PATH == NULL) {
        return -1;
    }
    strcpy(*PATH, initial_path);
    printf("NEW PATH: %s\n", *PATH);
    return 0;
}
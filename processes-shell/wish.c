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
int check_executable_path_validty(const char* executable_path, const char** pPATH);
bool is_absolute_path(const char* executable_path);

bool is_absolute_path(const char* executable_path) {
    // assume Linux path
    char *path_separator = "/";
    if (executable_path[0] == *path_separator) {
        return true;
    }
    return false;
}

const char* check_executable_path_validity(const char* executable_path, const char* PATH) {
    // immutability is important
    if (is_absolute_path(executable_path)) {
        if (access(executable_path, X_OK) == 0) {
            return executable_path;
        }
    }
    // only binary filename is given
    char* PATH_copy = strdup(PATH);
    const char* delimiter = ":";
    char* path;
    char* absolute_executable_path;
    // assume Linux path
    const char *path_separator = "/";
    while ((path = strsep(&PATH_copy, delimiter)) != NULL) {
        absolute_executable_path = (char *) malloc(strlen(path) + strlen(path_separator) + strlen(executable_path) + strlen("\0"));
        strcpy(absolute_executable_path, path);
        strcat(absolute_executable_path, path_separator);
        strcat(absolute_executable_path, executable_path);
        if (access(absolute_executable_path, X_OK) == 0) {
            return absolute_executable_path;
        }  
    }
    return NULL;

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
        // get the executable bin name, which is the first token
        const char* executable_path = strsep(&input, delimiter);
        if (executable_path == NULL) {
            printf("ERROR: no name of an executable file.\n");
            exit(1);
        }

        const char *absolute_executable_path = check_executable_path_validity(executable_path, PATH);
        // printf("absolute_executable: %s\n", absolute_executable_path);
        if (absolute_executable_path == NULL) {
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
    // remove newline character in the input
    if (input[nread - 1] == '\n') {
        input[nread - 1] = '\0';
    }
    // immutability is important
    char* input_copy = strdup(input);
    return input_copy;
}

int init_path_variable(char** PATH) {
    char* initial_path = "/bin:/usr/bin";
    *PATH = (char*) malloc(strlen(initial_path) + 1);
    if (*PATH == NULL) {
        return -1;
    }
    strcpy(*PATH, initial_path);
    printf("NEW PATH: %s\n", *PATH);
    return 0;
}
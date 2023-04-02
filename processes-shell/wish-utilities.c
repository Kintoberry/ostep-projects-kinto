
#include "wish-utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include "./wish-utilities.h"

const char *BUILT_IN_COMMANDS[] = {
    "exit",
    "cd",
    "path"
};
const size_t NUM_BUILT_IN_COMMANDS = 3;

bool is_absolute_path(const char* executable_path) {
    // assume Linux path
    char *path_separator = "/";
    if (executable_path[0] == *path_separator) {
        return true;
    }
    return false;
}

char* get_cwd() {
    size_t buffer_size = 256;
    char *buffer = (char *) malloc(buffer_size);
    while (getcwd(buffer, buffer_size) == NULL) {
        buffer_size = buffer_size * 2;
        buffer = realloc(buffer, buffer_size);
        if (buffer == NULL) {
            printf("ERROR: failed to reallocate a memory block for getting cwd.");
        }
    }
    return buffer;
}

char* check_executable_path_validity(char* executable_path, const char* PATH) {
    // Check if an absolute path
    if (is_absolute_path(executable_path)) {
        if (access(executable_path, X_OK) == 0) {
            return executable_path;
        } else {
            return NULL;
        }
    }
    // (Not absolute path) only binary filename is given
    // char* PATH_copy = strdup(PATH);
    const char* delimiter = ":";
    char* cwd = get_cwd();
    char* cwd_PATH_combined = malloc(strlen(cwd) + strlen(delimiter) + strlen(PATH) + strlen("\0"));
    strcpy(cwd_PATH_combined, cwd);
    strcat(cwd_PATH_combined, delimiter);
    strcat(cwd_PATH_combined, PATH);  
    char* path;
    char* absolute_executable_path;
    // assume Linux path
    const char *path_separator = "/";
    char* cwd_PATH_combined_copy = cwd_PATH_combined;
    while ((path = strsep(&cwd_PATH_combined_copy, delimiter)) != NULL) {
        absolute_executable_path = (char *) malloc(strlen(path) + strlen(path_separator) + strlen(executable_path) + strlen("\0"));
        strcpy(absolute_executable_path, path);
        strcat(absolute_executable_path, path_separator);
        strcat(absolute_executable_path, executable_path);
        if (access(absolute_executable_path, X_OK) == 0) {
            free(cwd_PATH_combined);
            return absolute_executable_path;
        }
        free(absolute_executable_path);  
    }
    return NULL;

}

bool is_built_in(const char* first_token) {
    for (int i = 0; i < NUM_BUILT_IN_COMMANDS; i++) {
        if (strcmp(first_token, BUILT_IN_COMMANDS[i]) == 0) {
            return true;
        }
    }
    return false;
}


int execute_built_in(char** tokens, size_t num_of_tokens)  {
    if (strcmp(tokens[0], "exit") == 0) {
        exit(0);
    }
    else if (strcmp(tokens[0], "cd")) {
        // need to be implemented 
        if (num_of_tokens > 2 || num_of_tokens == 0) {
            return -1;
        }
        int ret = chdir(tokens[1]);
        if (ret == -1) {
            return -1;
        }
        return 0;

    } else if (strcmp(tokens[0], "path")) {
        // need to be implemented
    }
    //
    return 0;
}

// NOTE: we don't deal with multiple spaces between tokens or trailing spaces
size_t get_token_nums(const char* input, const char* delimiter) {
    char* input_copy = strdup(input);
    char* original_input_copy = input_copy;
    size_t num_tokens = 0;
    char* token;
    while ((token =strsep(&input_copy, delimiter)) != NULL) {
        num_tokens++;
    }
    free(original_input_copy);
    return num_tokens;
}

char ** takeout_all_arguments(const char* input, const char* delimiter, size_t *num_of_tokens) {
    // separate out bin path and arguments that follows it
    char* input_copy = strdup(input);
    char* token;
    
    *num_of_tokens = get_token_nums(input, delimiter);
    if (num_of_tokens == 0) {
        return NULL;
    }
    // create 2D array
    char** arguments = (char**) malloc(sizeof(char*) * (*num_of_tokens));
    int index = 0;
    while ((token =strsep(&input_copy, delimiter)) != NULL) {
        arguments[index++] = token;
    }
    free(input_copy);
    return arguments;
}


int init_path_variable(char** PATH) {
    char* initial_path = "/bin:/usr/bin";
    *PATH = (char*) malloc(strlen(initial_path) + 1);
    if (*PATH == NULL) {
        return -1;
    }
    strcpy(*PATH, initial_path);
    return 0;
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
    free(input);
    return input_copy;
}

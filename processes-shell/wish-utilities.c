
#include "wish-utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include "./wish-utilities.h"
#include <sys/wait.h>
#include <sys/types.h>

static char *PATH = NULL;

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
            // printf("ERROR: failed to reallocate a memory block for getting cwd.");
            print_error();
        }
    }
    return buffer;
}

char* check_executable_path_validity(char* cmd_path, const char* PATH) {
    // Check if an absolute path
    if (is_absolute_path(cmd_path)) {
        if (access(cmd_path, X_OK) == 0) {
            return cmd_path;
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
        absolute_executable_path = (char *) malloc(strlen(path) + strlen(path_separator) + strlen(cmd_path) + strlen("\0"));
        strcpy(absolute_executable_path, path);
        strcat(absolute_executable_path, path_separator);
        strcat(absolute_executable_path, cmd_path);
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
    else if (strcmp(tokens[0], "cd") == 0) {
        // chdir only takes one argument. 
        if (num_of_tokens > 2 || num_of_tokens == 1) {
            return -1;
        }
        int ret = chdir(tokens[1]);
        if (ret == -1) {
            return -1;
        }
        return 0;

    } else if (strcmp(tokens[0], "path") == 0) {
        // need to be implemented
        if (num_of_tokens == 1) {
            set_path("");
        } else {
            size_t total_length = 0;
            for (int i = 1; i < num_of_tokens; i++) {
                total_length += strlen(tokens[i]);
            }
            char new_path[total_length];
            for(int i = 1; i < num_of_tokens; i++) {
                strcat(new_path, tokens[i]);
            }
            set_path(new_path);
        }
        return 0;
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
    // *num_of_tokens + 1 here for the NULL at the end of the array
    size_t arr_size = sizeof(char*) * (*num_of_tokens + 1);
    char** arguments = (char**) malloc(arr_size);
    int index = 0;
    while ((token =strsep(&input_copy, delimiter)) != NULL) {
        arguments[index++] = token;
    }
    arguments[index] = NULL;
    free(input_copy);
    return arguments;
}

int init_path(void) {
    char* initial_path = "/bin:/usr/bin";
    PATH = (char*) malloc(strlen(initial_path) + 1);
    if (PATH == NULL) {
        return -1;
    }
    strcpy(PATH, initial_path);
    return 0;
}
void set_path(char *new_path) {
    free(PATH);
    PATH = strdup(new_path);
}
const char * get_path(void) {
    return PATH;
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


char* receive_input(FILE* f_source) {
    char* input = NULL;
    size_t len = 0;
    ssize_t nread;
    nread = getline(&input, &len, f_source);
    if (nread == -1 && feof(stdin)) {
        // printf("EOF: end of stdin has reached.\n");
        print_error();
        exit(0);
    } else if (nread == -1 && ferror(stdin)) {
        // printf("ERROR: file error from stdin.\n");
        print_error();
        return NULL;
    }
    printf("strlen(input): %ld\n", strlen(input));
    // remove newline character in the input
    if (input[nread - 1] == '\n') {
        input[nread - 1] = '\0';
    }
    // immutability is important
    char* input_copy = strdup(input);
    free(input);
    return input_copy;
}

char* preprocess_input(char *input) {
    return "";
}

char* strip(char *input) {
    char *input_copy = strdup(input);
    // remove spaces and tab from the end
    int index = strlen(input_copy) - 1;
    while(index > 0) {
        if (input_copy[index] == ' ' || input_copy[index] == '\t') {
            input_copy[index] = '\0';
        } else {
            break;
        }
        index--;
    }
    
    // it removes space and tab from the begining and end of the string
    index = 0;
    int original_length = strlen(input_copy);
    int front_count = 0;
    while (index < original_length) {
        if (input_copy[index] == ' ' || input_copy[index] == '\t') {
            front_count++;
        } else {
            break;
        }
        index++;
    }
    memmove(input_copy, input_copy + front_count, strlen(input_copy) + 1 - front_count);
    return input_copy;
}

int execute_input(char *input) {
    size_t num_of_tokens;
    char **tokens = parse_input(input, &num_of_tokens);

    // check for a built-in command
    if (is_built_in(tokens[0])) {
        if ((execute_built_in(tokens, num_of_tokens) == -1)) {
            print_error();
            return -1;
        }
        return 0;
    } else {
        return execute(tokens);
    }
}

int execute(char **cmd_and_args) {
    // if (is_redirection(cmd_and_args)) {
    //     // set stdout
    // }
    char* cmd_absolute_path = check_executable_path_validity(cmd_and_args[0], get_path());
    pid_t pid = fork();
    if (pid < 0) {
        return -1;
    } else if (pid == 0) {
        // execv()
        if ((execv(cmd_absolute_path, cmd_and_args)) == -1) {
            return -1;
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
    return 0;

}

char** parse_input(char* input, size_t *num_of_tokens) {
    char* input_original = input;
    const char* delimiter = " ";
    
    char** tokens = takeout_all_arguments(input, delimiter, num_of_tokens);
    
    free(input_original);
    return tokens;
}

void print_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}
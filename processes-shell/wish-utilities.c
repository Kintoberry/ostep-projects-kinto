
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

int execute(Command *cmd);
char **parse_each_command(char *command_str, size_t num_items);
void empty_path();
void overwrite_path(char **paths, int start_index);
char *get_absolute_path(char *path); 


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


int execute_built_in(char** tokens)  {
    int index = 0;
    int count = 0;
    while (tokens[index++] != NULL) {
        count++;
    }
    size_t num_of_tokens = count;
    if (strcmp(tokens[0], "exit") == 0) {
        if (num_of_tokens != 1) {
            print_error();
        }
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
            empty_path();
        } else {
            overwrite_path(tokens, 1);
        }
        return 0;
    }
    return 0;
}

void overwrite_path(char **paths, int start_index) {
    size_t total_length = 0;
    for (int i = start_index; paths[i] != NULL; i++) {
        char *path;
        if (is_absolute_path(paths[i])) {
            path = get_absolute_path(paths[i]);
        } else {
            path = paths[i];
        }
        total_length = total_length + strlen(path) + strlen(":");
    }
    total_length += strlen("\0");
    char *new_PATH = malloc(total_length);
    for (int i = start_index; paths[i] != NULL; i++) {
        char *path;
        if (is_absolute_path(paths[i])) {
            path = get_absolute_path(paths[i]);
        } else {
            path = paths[i];
        }
        strcat(new_PATH, path);
        strcat(new_PATH, ":");
    }
    strcat(new_PATH, "\0");
    free(PATH);
    PATH = new_PATH;
}

char *get_absolute_path(char *path) {
    char *cwd = get_cwd();
    char *abs_path = malloc(strlen(cwd) + strlen(path) + strlen("\0"));
    strcat(abs_path, cwd);
    strcat(abs_path, path);
    strcat(abs_path, "\0");
    return abs_path;
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




char **separate_with_delimiter(const char* input, const char* delimiter, size_t *num_of_tokens) {
    // separate out bin path and arguments that follows it
    char* input_copy = strdup(input);
    char* token;
    
    *num_of_tokens = get_token_nums(input_copy, delimiter);
    if (num_of_tokens == 0) {
        return NULL;
    }
    // *num_of_tokens + 1 here for the NULL at the end of the array
    size_t arr_size = sizeof(char*) * (*num_of_tokens + 1);
    char** arguments = (char**) malloc(arr_size);
    int index = 0;
    while ((token =strsep(&input_copy, delimiter)) != NULL) {
        arguments[index++] = strip(token);
    }
    arguments[index] = NULL;
    free(input_copy);
    return arguments;
}

int init_path(void) {
    char* initial_path = "/bin:";
    PATH = (char*) malloc(strlen(initial_path) + 1);
    if (PATH == NULL) {
        return -1;
    }
    strcpy(PATH, initial_path);
    return 0;
}
void empty_path() {
    free(PATH);
    PATH = strdup("");
}
const char * get_path(void) {
    return PATH;
}


char* receive_input(FILE* f_source) {
    char* input = NULL;
    size_t len = 0;
    ssize_t nread;
    nread = getline(&input, &len, f_source);
    if (nread == -1 && feof(stdin)) {
        exit(0);
    } else if (nread == -1 && ferror(stdin)) {
        // printf("ERROR: file error from stdin.\n");
        // print_error();
        return NULL;
    } else if (nread == -1) {
        return NULL;
    }
    // remove newline character in the input
    if (input[nread - 1] == '\n') {
        input[nread - 1] = '\0';
    }
    return input;
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

char **separate_parallel_commands(char *input, size_t *num_items) {
    return separate_with_delimiter(input, "&", num_items);
}

int parse_redirection(Command* cmd, char *cmd_str) {
    size_t num_items;
    char **tokens = separate_with_delimiter(cmd_str, ">", &num_items);
    if (num_items > 2) {
        print_error();
        return -1;
    } else if (num_items == 2) {
        cmd->is_redirection = true;
        cmd->redirection_fp = fopen(tokens[1], "w");
    } else {
        cmd->is_redirection = false;
        cmd->redirection_fp = NULL;
    }
    cmd->cmd_and_args = parse_each_command(tokens[0], (cmd->num_items));
    return 0;
}

char **parse_each_command(char *command_str, size_t num_items) {
    char **arr_of_cmds_and_args = malloc(sizeof(char *) * (num_items + 1));
    // for (int i = 0; i < num_items; i++) {
    // }
    size_t num_tokens;
    arr_of_cmds_and_args = separate_with_delimiter(command_str, " ", &num_tokens);
    arr_of_cmds_and_args[num_tokens] = NULL;
    return arr_of_cmds_and_args;
}

Command **parse_all_commands(char *input) {
    size_t num_parallel_cmds;
    char **parallel_cmds = separate_parallel_commands(input, &num_parallel_cmds);
    Command **cmds = (Command **) malloc(sizeof(Command **) * (num_parallel_cmds + 1));
    int index = 0;
    while (parallel_cmds[index] != NULL) {
        Command *each_cmd = (Command *) malloc(sizeof(Command *));
        parse_redirection(each_cmd, parallel_cmds[index]);
        cmds[index] = each_cmd;
        index++;
    }
    cmds[index] = NULL;
    return cmds;
}

int execute_input(char *input) {
    Command **cmds = parse_all_commands(strip(input));
    int index = 0;
    while(cmds[index] != NULL) {
        if (execute(cmds[index]) == -1) {
            return -1;
        }
        index++;
    }
    int status;
    while (wait(&status) != -1) {
        ;
    }
    int i = 0;
    while (cmds[i] != NULL) {
        free(cmds[i++]);
    }
    free(cmds);
    return 0;
}

int execute(Command *cmd) {
    int original_stdout = dup(STDOUT_FILENO);
    if (cmd->is_redirection) {
        dup2(fileno(cmd->redirection_fp), STDERR_FILENO);
    }

    if (is_built_in(cmd->cmd_and_args[0])) {
        if ((execute_built_in(cmd->cmd_and_args) == -1)) {
            print_error();
            return -1;
        }
        return 0;
    }
    char* cmd_absolute_path = check_executable_path_validity(cmd->cmd_and_args[0], get_path());
    pid_t pid = fork();
    if (pid < 0) {
        return -1;
    } else if (pid == 0) {
        // execv()
        if ((execv(cmd_absolute_path, cmd->cmd_and_args)) == -1) {
            print_error();
            return -1;
        }
    } else {
        // int status;
        // waitpid(pid, &status, 0);
    }
    if (cmd->is_redirection) {
        fflush(stdout);
        dup2(original_stdout, STDERR_FILENO);
    }
    return 0;
}


void print_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}
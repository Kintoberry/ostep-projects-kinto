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
char* get_cwd();
bool is_built_in(const char* target);
void execute_built_in(const char* built_in_filename); 
char ** takeout_all_arguments(const char* input, const char* delimiter);
size_t get_token_nums(const char* input, const char* delimiter);

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

const char* check_executable_path_validity(const char* executable_path, const char* PATH) {
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

bool is_built_in(const char* target) {
    for (int i = 0; i < NUM_BUILT_IN_COMMANDS; i++) {
        if (strcmp(target, BUILT_IN_COMMANDS[i]) == 0) {
            return true;
        }
    }
    return false;
}

void execute_built_in(const char* built_in_filename)  {
    if (strcmp(built_in_filename, "exit") == 0) {
        exit(0);
    }
    else if (strcmp(built_in_filename, "cd")) {
        // need to be implemented 
    } else if (strcmp(built_in_filename, "path")) {
        // need to be implemented
    }
}

size_t get_token_nums(const char* input, const char* delimiter) {
    char* input_copy = strdup(input);
    size_t num_tokens = 0;
    char* token;
    if ((token =strsep(&input_copy, delimiter)) != NULL) {
        num_tokens++;
    }
    free(input_copy);
    return num_tokens;
}

char ** takeout_all_arguments(const char* input, const char* delimiter) {
    // separate out bin path and arguments that follows it
    char* input_copy = strdup(input);
    char* token;
    
    size_t num_tokens = get_token_nums(input, delimiter);
    // create 2D array
    char** arguments = (char**) malloc(sizeof(char*) * num_tokens);
    int index = 0;
    while ((token =strsep(&input_copy, delimiter)) != NULL) {
        arguments[index++] = token;
    }
    free(input_copy);
    return arguments;
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
        char* input_original = input;
        const char* delimiter = " ";
        char* token;
        // get the executable bin name, which is the first token
        const char* executable_path = strsep(&input, delimiter);
        if (executable_path == NULL) {
            printf("ERROR: no name of an executable file.\n");
            exit(1);
        }

        // check for a built-in command
        if (is_built_in(executable_path)) {
            execute_built_in(executable_path);
            free(input_original);
            continue;
        }

        const char *absolute_executable_path = check_executable_path_validity(executable_path, PATH);
        // printf("absolute_executable: %s\n", absolute_executable_path);
        if (absolute_executable_path == NULL) {
            printf("ERROR: cannot access the executable.\n");
            // exit(1);
            free(input_original);
            continue;
        }
        
        while ((token = strsep(&input, delimiter)) != NULL) {
            printf("inputs: %s\n", token);
        }

        free(input_original);
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
    free(input);
    return input_copy;
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
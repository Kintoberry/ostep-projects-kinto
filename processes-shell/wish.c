#include <unistd.h>
#include <fcntl.h>
#include "./wish-utilities.h"

char* PATH = NULL;
size_t PATH_LEN = 0;


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


#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "./wish-utilities.h"

char* PATH = NULL;
size_t PATH_LEN = 0;


int main(int argc, char** argv) {
    if (argc > 2) {
        printf("ERROR: Bad usage.\n");
        exit(1);
    }
    int err = init_path();
    if (err == -1) {
        printf("ERROR: PATH initionalization failed.\n");
        exit(1);
    }
    
    if (argc == 2) {
        // batch mode
        FILE* fp = fopen(argv[1], "r");
        char *input_from_batch;
        while ((input_from_batch = receive_input(fp)) != NULL) {
            execute_input(input_from_batch);

        }
        fclose(fp);
    }
    
    while(true) {
        printf("wish> ");
        char* input = receive_input(stdin);
        if (input == NULL) {
            printf("ERROR: couldn't read the input.\n");
        }
        execute_input(input);

        // char *absolute_executable_path = check_executable_path_validity(executable_path, PATH);
        // // printf("absolute_executable: %s\n", absolute_executable_path);
        // if (absolute_executable_path == NULL) {
        //     printf("ERROR: cannot access the executable.\n");
        //     // exit(1);
        //     free(input_original);
        //     continue;
        // }
        

    }
}


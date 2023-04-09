#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "./wish-utilities.h"

char* PATH = NULL;
size_t PATH_LEN = 0;


int main(int argc, char** argv) {
    if (argc > 2) {
        // printf("ERROR: Bad usage.\n");
        print_error();
        exit(1);
    }
    bool is_batch_mode = false;
    if (argc == 2) {
        is_batch_mode = true;
    }
    int err = init_path();
    if (err == -1) {
        // printf("ERROR: PATH initionalization failed.\n");
        print_error();
        exit(1);
    }

    FILE *fp;
    if (is_batch_mode) {
        fp = fopen(argv[1], "r");  
    } else {
        fp = stdin;
        printf("wish> ");
    }
    char* input;
    
    while((input = receive_input(fp)) != NULL) {
        // printf("input: %s\n", input);
        if (!is_batch_mode)
            printf("wish> ");
        if (input == NULL) {
            // printf("ERROR: couldn't read the input.\n");
            print_error();
        }
        execute_input(input);
    }
    if (fp != stdout) {
        fclose(fp);
    }
}


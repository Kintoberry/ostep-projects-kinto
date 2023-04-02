#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>

char* process_input(FILE* f_source);
int init_path_variable(char** PATH);
char* check_executable_path_validity(char* executable_path, const char* PATH);
bool is_absolute_path(const char* executable_path);
char* get_cwd();
bool is_built_in(const char* target);
int execute_built_in(char** tokens, size_t num_of_tokens); 
char ** takeout_all_arguments(const char* input, const char* delimiter, size_t *num_of_tokens);

int init_path(void);
void set_path(char *new_path);
const char * get_path(void);

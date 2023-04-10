#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>

char* receive_input(FILE* f_source);
int init_path_variable(char** PATH);
char* check_executable_path_validity(char* cmd_path, const char* PATH);
bool is_absolute_path(const char* executable_path);
char* get_cwd();
bool is_built_in(const char* target);
int execute_built_in(char** tokens); 
char ** separate_with_delimiter(const char* input, const char* delimiter, size_t *num_of_tokens);
char ***parse_each_parallel_command(char **parallel_commands, size_t num_items);
int init_path(void);
void set_path(char *new_path);
const char * get_path(void);
char** parse_input(char *input, size_t *num_of_tokens);
int execute_input(char *input);
int execute(char **cmd_and_args);
void print_error();
char* preprocess_input(char *input);
char* strip(char *input);
char ***separate_and_parse_parallel_commands(char *input);
char **separate_parallel_commands(char *input, size_t *num_items);
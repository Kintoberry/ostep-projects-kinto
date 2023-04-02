#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>

char* process_input(FILE* f_source);
int init_path_variable(char** PATH);
int check_executable_path_validty(const char* executable_path, const char** pPATH);
bool is_absolute_path(const char* executable_path);
char* get_cwd();
bool is_built_in(const char* target);
void execute_built_in(const char* built_in_filename); 
char ** takeout_all_arguments(const char* input, const char* delimiter);
size_t get_token_nums(const char* input, const char* delimiter);


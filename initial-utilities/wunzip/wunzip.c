#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define BUFFER_SIZE 1000

typedef struct prev_memory {
    int previous_count;
    char previous_char;
} prev_record_t;

void uncompress(FILE *fp);
bool concat_files(char** file_paths, int num_files, FILE* output_path);


bool concat_files(char** file_paths, int num_files, FILE* fp_out) {
    for (int i = 0; i < num_files; i++) {
        FILE *fp = fopen(file_paths[i], "rb");
        if (fp == NULL) {
            return false;
        }
        size_t bytes_read;
        char buffer[1024];
        while((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
            fwrite(buffer, 1, bytes_read, fp_out);
        }
        fclose(fp);
    }
    return true;
}

void uncompress(FILE *fp) {
    typedef struct chunk {
        int32_t count;
        char ascii_char;
    } Chunk;

    // char* buffer = malloc(sizeof(char) * BUFFER_SIZE);
    char buffer[BUFFER_SIZE];
    size_t nread;
    Chunk data;
    while ((nread = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) { 
        // printf("reading: nread %ld\n", nread);
        for (int i = 0; i < nread; i = i + 5) {
            data.count = *(int *)(buffer + i);
            data.ascii_char = buffer[i + 4];
            // printf("count: %d\n char: %c\n", data.count, data.ascii_char);
            for (int j = 0; j < data.count; j++) {
                printf("%c", data.ascii_char);
            }
        }
    }
    // printf("nread: %ld\n", nread);
    
}

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

    for (int i = 1; i < argc; i++ ) {
        FILE* fp = fopen(argv[i], "rb");
        if (fp == NULL) {
            printf("ERROR: cannot create a new file.\n");
            printf("%s\n", strerror(errno));
            exit(1);
        }
        uncompress(fp);
        fclose(fp);
    }
    fclose(fp_combined);
}


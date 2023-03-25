#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define BUFFER_SIZE 1024

void compress(char* buffer, size_t nread, FILE *fp);

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

void compress(char* buffer, size_t nread, FILE *fp) {
    struct chunk {
        int32_t count;
        char ascii_char;
    };
    int curr_pos = 0;
    int forward_pos = curr_pos + 1;
    while (curr_pos < nread) {
        int count = 1;
        while (forward_pos < nread) {
            if (buffer[forward_pos] != buffer[curr_pos]) {
                break;
            }
            count++;
            forward_pos++;
            curr_pos++;
        }
        // write out the compressed data to stdout
        // TODO: maybe ditch struct and use bit manipulation
        struct chunk count_ascii_pair;
        count_ascii_pair.count = count;
        count_ascii_pair.ascii_char = buffer[curr_pos];
        fwrite(&count_ascii_pair, 1, 5, stdout);
        curr_pos++;
        forward_pos++;
    }
}

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }
    char* combined_filename = "out_concat.txt";
    int ret = remove(combined_filename);
    if (ret > 0) {
        printf("ERROR: canot remove the combined file.");
        printf("%s\n", strerror(errno));
        return 1;
    }
    FILE* fp_combined = fopen(combined_filename, "a+b");
    if (fp_combined == NULL) {
        printf("ERROR: cannot create a new file.\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }
    bool res = concat_files(&argv[1], argc - 1, fp_combined);
    if (!res) {
        printf("ERROR: cannot concatenate files.\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }
    if (fseek(fp_combined, 0, SEEK_SET) != 0) {
        printf("ERROR: cannot put the file position indicator to the beginning.\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }
    // char* buffer = malloc(sizeof(char) * BUFFER_SIZE);
    char buffer[BUFFER_SIZE];
    size_t nread;

    while ((nread = fread(buffer, 1, BUFFER_SIZE, fp_combined)) > 0) { 
        compress(buffer, nread, stdout); 
    }


    // free(buffer);
    fclose(fp_combined);
}


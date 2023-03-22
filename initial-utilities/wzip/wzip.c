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
    printf("Hello!!");
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
        struct chunk count_ascii_pair;
        count_ascii_pair.count = count;
        count_ascii_pair.ascii_char = buffer[curr_pos];
        printf("count? %d", count_ascii_pair.count);
        fwrite(&count_ascii_pair, sizeof(struct chunk), 1, stdout);
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
    FILE* fp_combined = fopen(combined_filename, "ab");
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

    char* buffer = malloc(sizeof(char) * BUFFER_SIZE);
    size_t nread;
    while ((nread = fread(buffer, 1, BUFFER_SIZE, fp_combined)) > 0) { 
        printf("before fread:");
        compress(buffer, nread, stdout); 
    }
    printf("nread: %ld", nread);


    free(buffer);
    fclose(fp_combined);
}


// int compress2(char* buffer, size_t nread, FILE *fp) {
//     int index = 0;
//     int count = 1;
//     while (index < nread) {
//         char current_char = buffer[index];
//         while (index + 1 < nread) {
//             current_char = buffer[index];
//             if (current_char != buffer[index + 1]) {
//                 char *temp = malloc(5);
//                 temp[0] = (count >> 0) & 0xFF;
//                 temp[1] = (count >> 8) & 0xFF;
//                 temp[2] = (count >> 16) & 0xFF;
//                 temp[3] = (count >> 24) & 0xFF;
//                 temp[4] = current_char;
//                 fwrite(temp, 1, 5, fp);
//                 free(temp);
//                 break;
//             }
//             count++;
//             index++;
//         }
//         index++;
//     }
//     return count;
// }
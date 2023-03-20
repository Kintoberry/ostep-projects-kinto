#include <stdio.h>
#include <stdlib.h>
#define BUFFER_SIZE 10000

int compress(char* buffer, size_t nread, FILE *fp, int count);


int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) { 
        char *fileName = argv[i];
        char* buffer = malloc(BUFFER_SIZE);
        FILE *fp = fopen(fileName, "rb");
        FILE *outFilePointer = stdout;
        // FILE *outFilePointer = fopen("output.txt", "wb");
        if (fp == NULL) {
            printf("Error opening the file\n");
        }
        size_t nread;
        int count;
        while ((nread = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) { 
            if (count > 0)
                count = compress(buffer, nread, outFilePointer, count);
            else 
                count = compress(buffer, nread, outFilePointer, 1);   
        }
        free(buffer);
        fclose(fp);
        // fclose(outFilePointer);
    }
}

int compress(char* buffer, size_t nread, FILE *fp, int count) {
    int index = 0;
    while (index < nread) {
        char current_char = buffer[index];
        while (index + 1 < nread) {
            current_char = buffer[index];
            if (current_char != buffer[index + 1]) {
                char *temp = malloc(5);
                temp[0] = (count >> 0) & 0xFF;
                temp[1] = (count >> 8) & 0xFF;
                temp[2] = (count >> 16) & 0xFF;
                temp[3] = (count >> 24) & 0xFF;
                temp[4] = current_char;
                fwrite(temp, 1, 5, fp);
                free(temp);
                count = 1;
                break;
            }
            count++;
            index++;
        }
        index++;
    }
    return count;
}

// temp[0] = count;
        // printf("%d%c\n", temp[0], temp[4]);
        // size_t nwritten = fwrite(&buffer[index - count + 1], 1, count, stdout);
        // FILE *fop = fopen("output.txt", "wb");
        // if (nwritten < count) {
        //         printf("nwritten: %d\n", (int) nwritten);
        //         printf("ncount: %d\n", count);
        //         printf("Error has occurred while writing to a file.\n");
        //         exit(1);
        // }
        // for (int i = 0; i < 5; i++) {
        //     printf("byte %d: %hhu\n", i, temp[i]);
        // }
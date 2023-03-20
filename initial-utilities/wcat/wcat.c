
#include <stdio.h>
#include <stdlib.h>
void printFileContent(char* fileName);

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        printFileContent(argv[i]);
    }
    return 0;
}

void printFileContent(char* fileName) {
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("wcat: cannot open file\n");
        exit(1);
    }
    char *str = malloc(100 * sizeof(char));
    while(fgets(str, 100, fp) != NULL) {
        printf("%s", str);
    }
    fclose(fp);
}


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char** argv) {
    if (argc == 1) {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }
    if (argc == 2) {
        char* targetString = argv[1];
        char *str = malloc(100 * sizeof(char));
        while (fgets(str, 100, stdin) != NULL) {
            char *substr = strstr(str, targetString);
            if (substr == NULL) {
                continue;
            }
            printf("%s", str);
        }
    }
    if (argc == 3) {
        char *line = NULL;
        size_t len = 0;
        size_t nread;
        FILE *fp = fopen(argv[2], "r");
        if (fp == NULL) {
            printf("wgrep: cannot open file\n");
            exit(1);
        }
        while ((nread = getline(&line, &len, fp)) != -1) {
            char *substr = strstr(line, argv[1]);
            if (substr != NULL) {
                printf("%s", line);
            }
        }
        fclose(fp);
        free(line);
    }
}

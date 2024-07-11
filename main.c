#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mainloop.h"
#define PATH_SIZE 1024
#define MAX_LINE_LENGTH 128

int main(int argc, char *argv[]) {
    char *cw_path = malloc(MAX_LINE_LENGTH);
    if (cw_path == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if (argc > 1) {
        if (chdir(argv[1]) != 0) {
            perror(argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    cw_path = getcwd(cw_path, MAX_LINE_LENGTH);
    if (cw_path == NULL) {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    int status = mainloop(cw_path);

    free(cw_path);
    exit(status == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}


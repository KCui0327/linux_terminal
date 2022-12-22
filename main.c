#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INPUT_BUF_SIZE 1024
#define SPLIT_BUF_SIZE 64

void runTerminal(void);
char *readLine(void);
char **splitLine(char *line);

int main(int argc, char **argv) {
    runTerminal();
    return EXIT_SUCCESS;
}

void runTerminal(void) {
    char *line, **args;
    bool status;

    do {
        printf("> ");
        line = readLine();
        args = splitLine(line);
        status = execute(args);
        free(line);
        free(args);
    } while (status);
}

char *readLine(void) {
    int position = 0, bufferSize = INPUT_BUF_SIZE, c;
    char *buffer = malloc(sizeof(char) * bufferSize);

    if (!buffer) {
        printf("lsh: allocation error");
        exit(EXIT_FAILURE);
    }

    while (true) {
        c = getchar();
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        }

        buffer[position] = c;
        position++;

        if (position >= INPUT_BUF_SIZE) {
            bufferSize += INPUT_BUF_SIZE;
            buffer = realloc(buffer, bufferSize);
            if (!buffer) {
                printf("lsh: allocation error");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **splitLine(char *line) {
    int bufferSize = SPLIT_BUF_SIZE, position = 0;
    char **wordSplit = malloc(sizeof(char) * bufferSize);
    if (!wordSplit) {
        printf("lsh: allocation error");
        exit(EXIT_FAILURE);
    }

    char *token = strtok(line, )

}


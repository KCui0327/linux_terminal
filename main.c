#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define INPUT_BUF_SIZE 1024
#define SPLIT_BUF_SIZE 64
#define CHAR_SPLIT " -\t\r\n\a"

void runTerminal(void);
char *readLine(void);
char **splitLine(char *line);
int launch(char **args);
int cd(char **args);
int help(char **args);
int exitTerm(char **args);
int numBuiltIn(void);
int execute(char **args);

char *builtin_str[] = {"cd", "help", "exit"};
int (*builtin_func[]) (char **) = {&cd, &help, &exitTerm};

int main(int argc, char **argv) {
    runTerminal();
    return EXIT_SUCCESS;
}

void runTerminal(void) {
    char *line, **args;
    int status;

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
        fprintf(stderr, "lsh: allocation error");
        exit(EXIT_FAILURE);
    }

    while (true) {
        c = getchar();
        if (c == EOF) exit(EXIT_SUCCESS);
        else if(c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        if (position >= INPUT_BUF_SIZE) {
            bufferSize += INPUT_BUF_SIZE;
            buffer = realloc(buffer, bufferSize);
            if (!buffer) {
                fprintf(stderr, "lsh: allocation error");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **splitLine(char *line) {
    int bufferSize = SPLIT_BUF_SIZE, position = 0;
    char **wordSplit = malloc(sizeof(char) * bufferSize);

    if (!wordSplit) {
        fprintf(stderr, "lsh: allocation error");
        exit(EXIT_FAILURE);
    }

    char *token = strtok(line, CHAR_SPLIT);
    char **backupToken;

    while (token != NULL) {
        wordSplit[position] = token;
        position++;

        if (position >= bufferSize) {
            bufferSize += SPLIT_BUF_SIZE;
            backupToken = wordSplit;
            wordSplit = realloc(wordSplit, sizeof(char *) * bufferSize);
            if (!wordSplit) {
                free(backupToken);
                fprintf(stderr, "lsh: allocation error");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, CHAR_SPLIT);
    }

    wordSplit[position] = NULL;
    return wordSplit;
}

int launch(char **args) {
    pid_t pid;
    int status;
    pid = fork();

    if (pid == 0) {
        if (execvp(args[0], args) == -1) perror("lsh");
        exit(EXIT_FAILURE);
    } else if (pid < 0) perror("lsh");
    else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int numBuiltIn(void) {return sizeof(builtin_str) / sizeof(char *);}

int cd(char **args) {
    if (args[1] == NULL) fprintf(stderr,"lsh: expected argument to \"cd\"\n");
    else {
        if (chdir(args[1]) != 0) {
            perror("lsh");
        }
    }
    return 1;
}

int help(char **args) {
    printf("Kenny Cui's LSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (int i = 0; i < numBuiltIn(); i++)
        printf("    %s\n", builtin_str[i]);

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int exitTerm(char ** args) {return 0;}

int execute(char **args) {
    if (args[0] == NULL) return 1;

    for (int i = 0; i < numBuiltIn(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0)
            return (*builtin_func[i])(args);
    }
    return launch(args);
}

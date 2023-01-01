#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#define INPUT_BUF_SIZE 1024
#define SPLIT_BUF_SIZE 64
#define CHAR_SPLIT " -\t\r\n\a"

void runTerminal(void);
char *readLine(void);
char **splitLine(char *line);
int launch(char **args);
int cd(char **args);
int pwd(char **args);
int ls(char **args);
int mkDirectory(char **args);
int rmDirectory(char **args);
int wget(char **args);
int help(char **args);
int copyFile(char **args);
int moveFile(char **args);
int removeFile(char **args);
int clear(char **args);
int exitTerm(char **args);
int numBuiltIn(void);
int execute(char **args);

// default commands
char *builtin_str[] = {
        "cd",
        "pwd",
        "ls",
        "mkdir",
        "rmdir",
        "wget",
        "rm",
        "cp",
        "mv",
        "help",
        "clear",
        "exit"
};
int (*builtin_func[]) (char **) = {
        &cd,
        &pwd,
        &ls,
        &mkDirectory,
        &rmDirectory,
        &wget,
        &copyFile,
        &moveFile,
        &removeFile,
        &help,
        &clear,
        &exitTerm
};

int main(int argc, char **argv) {
    printf("Welcome to the Linux Terminal!\n\n");
    runTerminal();
    return EXIT_SUCCESS;
}

// run terminal
void runTerminal(void) {
    char *line, **args;
    int status;

    do { // continuously prompts for input
        printf("> ");
        line = readLine();
        args = splitLine(line);
        status = execute(args);
        free(line);
        free(args);
    } while (status);
}

// read input
char *readLine(void) {
    int position = 0, bufferSize = INPUT_BUF_SIZE, c;
    char *buffer = malloc(sizeof(char *) * bufferSize);

    if (!buffer) { // cannot allocate memory
        fprintf(stderr, "lsh: allocation error");
        exit(EXIT_FAILURE);
    }

    while (true) {
        c = getchar();
        if (c == EOF) exit(EXIT_SUCCESS); // reached end of file
        else if (c == '\n') { // reached end of line
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // if input exceeds buffer size
        if (position >= INPUT_BUF_SIZE) {
            bufferSize += INPUT_BUF_SIZE;
            buffer = realloc(buffer, bufferSize); // reallocate with double the size
            if (!buffer) { // cannot allocate memory
                fprintf(stderr, "lsh: allocation error");
                exit(EXIT_FAILURE);
            }
        }
    }
}

// parse input
char **splitLine(char *line) {
    int bufferSize = SPLIT_BUF_SIZE, position = 0;
    char **wordSplit = malloc(sizeof(char *) * bufferSize);

    if (!wordSplit) { // cannot allocate memory
        fprintf(stderr, "lsh: allocation error");
        exit(EXIT_FAILURE);
    }

    char *token = strtok(line, CHAR_SPLIT); // split inputs into tokes
    char **backupToken;

    while (token != NULL) {
        wordSplit[position] = token;
        position++;

        // if input exceeds buffer size
        if (position >= bufferSize) {
            bufferSize += SPLIT_BUF_SIZE;
            backupToken = wordSplit;
            wordSplit = realloc(wordSplit, sizeof(char *) * bufferSize); // reallocate with double the size
            if (!wordSplit) { // cannot allocate memory
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

// launches terminal
int launch(char **args) {
    pid_t pid;
    int status;
    pid = fork();

    if (pid == 0) { // returned to newly created child
        if (execvp(args[0], args) == -1) perror("lsh");
        exit(EXIT_FAILURE);
    } else if (pid < 0) perror("lsh"); // cannot create child
    else { // return to parent
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

// number of built-in commands
int numBuiltIn(void) { return sizeof(builtin_str) / sizeof(builtin_str[0]);}

// cd command
int cd(char **args) {
    if (args[1] == NULL) fprintf(stderr,"lsh: expected argument to \"cd\"\n"); // no argument given
    if (chdir(args[1]) != 0) perror("lsh");
    return 1;
}

// pwd command
int pwd(char **args) {
    char s[1024];
    if (getcwd(s, sizeof(s)) == NULL) perror("pwd");
    printf("%s\n", s); // prints current dir

    return 1;
}

// ls command
int ls(char **args) {
    pid_t pid = fork(); // create process to execute command
    int status;

    if (pid == 0) {
        execvp("ls", args); // execute command
        perror("ls");
        exit(EXIT_FAILURE);
    } else if (pid < 0) perror("fork"); // fork error
    waitpid(pid, &status, 0); // wait for child process to finish
    return 1;
}

// mkdir command
int mkDirectory(char **args) {
    if (args[1] == NULL) fprintf(stderr, "mkDirectory: expected argument\n");
    if (mkDirectory(args[1]) != 0) perror("mkDirectory");

    return 1;
}

// rmdir command
int rmDirectory(char **args) {
    if (args[1] == NULL) fprintf(stderr, "rmDirectory: expected argument");
    if (rmDirectory(args[1]) != 0) perror("rmDirectory");

    return 1;
}

// wget command
int wget(char **args) {
    pid_t pid = fork(); // create process to execute command
    int status;

    if (pid == 0) {
        execvp("wget", args); // execute command
        perror("wget");
        exit(EXIT_FAILURE);
    } else if (pid < 0) perror("fork"); // fork error
    waitpid(pid, &status, 0); // wait for child process to finish

}

// copy file
int copyFile(char **args) {
    if (args[1] == NULL || args[2] == NULL)  // missing arguments
        fprintf(stderr, "lsh: two few arguments");
    else {
        int status;
        // allocate memory for command
        char *command = malloc(sizeof(char) * (strlen(args[1]) + strlen(args[2])) + 4);
        sprintf(command, "cp %s, %s", args[1], args[2]); // copy arguments into command
        status = system(command); // executes command
        free(command); // free dynamically allocated memory for command
        if (status != 0) // error occurred
            fprintf(stderr, "lsh: error in copying files");
    }
    return 1;
}

// move file
int moveFile(char **args) {
    if (args[1] == NULL || args[2] == NULL)  // missing arguments
        fprintf(stderr, "lsh: two few arguments");
    else {
        int status;
        // allocate memory for command
        char *command = malloc(sizeof(char) * (strlen(args[1]) + strlen(args[2])) + 4);
        sprintf(command, "mv %s, %s", args[1], args[2]); // copy arguments into command
        status = system(command); // executes command
        free(command); // free dynamically allocated memory for command
        if (status != 0) // error occurred
            fprintf(stderr, "lsh: error in moving files");
    }
    return 1;
}

// delete file
int removeFile(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: two few arguments"); // missing arguments
    } else {
        // attempt to delete each file
        for (int i = 0; args[i] != NULL; i++) {
            if (unlink(args[i]) != 0) perror("lsh"); // specified file was not removed
        }
    }
    return 1;
}

// help command
int help(char **args) {
    printf("Kenny Cui's LSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    // print the names of the built-in commands
    for (int i = 0; i < numBuiltIn(); i++)
        printf("    %s\n", builtin_str[i]);

    printf("Use the man command for information on other programs.\n");
    return 1;
}

// clear terminal
int clear(char **args) {
    if (args[1] != NULL) fprintf(stderr, "too many arguments\n"); // too many arguments
    printf("\0333c"); // clear screen
    return 1;
}

// exit command
int exitTerm(char ** args) {return 0;}

// execute commands
int execute(char **args) {
    if (args[0] == NULL) return 1; // no command specified

    // check if the command is a built-in command
    for (int i = 0; i < numBuiltIn(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            // execute the corresponding function for the built-in command
            return (*builtin_func[i])(args);
        }
    }
    // command is not a built-in command, launch it in a new process
    return launch(args);
}


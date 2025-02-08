#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#define MAX_INPUT_CHARS 500
#define MAX_INPUT_WORDS 20

void changeDirectories(const char *path);
int executeCommand(char *const *enteredCommand, const char *infile, const char *outfile);
int parseInput(char *input, char splitWords[][MAX_INPUT_CHARS], int maxWords);

int main()
{
    // Set up prompt
    const char netid[] = "mongkingco";
    char cwd[MAX_INPUT_CHARS];

    char input[MAX_INPUT_CHARS];
    char splitWords[MAX_INPUT_WORDS][MAX_INPUT_CHARS];

    // Main loop
    while (1)
    {
        // Display prompt
        getcwd(cwd, MAX_INPUT_CHARS);
        printf("%s:%s$ ", netid, cwd);

        // Get Command Line Input (CLI)
        fgets(input, MAX_INPUT_CHARS, stdin);
        int numWords = parseInput(input, splitWords, MAX_INPUT_WORDS);

        // Interpet CLI
        char *command = splitWords[0];

        // Check change directory command
        if (strcmp("cd", command) == 0)
        {
            if (numWords == 2)
            {
                changeDirectories(splitWords[1]);
                continue;
            }
            else
            {
                printf("Path Not Formatted Correctly! \n");
                continue;
            }
        }

        // Check exit command
        if (strcmp("exit", command) == 0)
        {
            break;
        }

        // Check for redirection characters
        char *infile = NULL;
        char *outfile = NULL;

        for (int i = 0; i < numWords; i++)
        {
            if (strcmp(splitWords[i], ">") == 0) // Output
            {
                outfile = splitWords[i + 1];
                numWords = i;
                printf("DETECTED REDIRECT \n");
                break;
            }
            if (strcmp(splitWords[i], "<") == 0) // Input
            {
                infile = splitWords[i + 1];
                numWords = i;
                break;
            }
        }

        // Dynamically copy new array of string args
        char **args = (char **)malloc((numWords + 1) * sizeof(char *));
        for (int i = 0; i < numWords; i++)
        {
            args[i] = strdup(splitWords[i]);
        }
        args[numWords] = NULL; // Null terminate string array

        // Execute command with redirection
        executeCommand(args, infile, outfile);
    }

    return 0;
}

void changeDirectories(const char *path)
{
    if (chdir(path) == -1)
        printf("chdir Failed: %s", strerror(errno));
}

int executeCommand(char *const *enteredCommand, const char *infile, const char *outfile)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        printf("fork Failed: %s \n", strerror(errno));
        return -1;
    }

    if (pid == 0) // Child
    {
        // Check redirection
        if (outfile != NULL && strcmp(outfile, "") != 0)
        {
            int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            dup2(fd, STDOUT_FILENO);
        }
        if (infile != NULL && strcmp(infile, "") != 0)
        {
            int fd = open(infile, O_RDONLY, 0666);
            dup2(fd, STDIN_FILENO);
        }

        // Continue new process
        int success = execvp(enteredCommand[0], enteredCommand);
        if (success == -1)
        {
            printf("exec Failed: %s ]n", strerror(errno));
            _exit(1);
        }
    }

    else // Parent
    {
        int status;
        wait(&status);
        printf("Child finished with error status: %d \n", status);
    }

    return 0;
}

int parseInput(char *input, char splitWords[][MAX_INPUT_CHARS], int maxWords)
{
    int numWords = 0;
    const char *delim = " \n";
    char *nextWord = strtok(input, delim);

    while (nextWord != NULL && numWords < maxWords)
    {
        strcpy(splitWords[numWords], nextWord);
        nextWord = strtok(NULL, delim);
        numWords++;
    }

    return numWords;
}
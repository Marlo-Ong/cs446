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
void executeCommand(char *const *enteredCommand, const char *infile, const char *outfile);
int parseInput(char *input, char splitWords[][500], int maxWords);

int main()
{
    // Set up prompt
    const char netid[] = "mongkingco";
    char cwd[MAX_INPUT_CHARS];

    // Main loop
    while (1)
    {
        // Display prompt
        getcwd(cwd, MAX_INPUT_CHARS);
        printf("%s:%s$ ", netid, cwd);

        // Get Command Line Input (CLI)
        char input[MAX_INPUT_CHARS];
        char splitWords[MAX_INPUT_WORDS][MAX_INPUT_CHARS];

        fgets(input, MAX_INPUT_CHARS, stdin);
        int numWords = parseInput(input, splitWords, MAX_INPUT_WORDS);
    }

    return 0;
}

void changeDirectories(const char *path)
{
}

void executeCommand(char *const *enteredCommand, const char *infile, const char *outfile)
{
}

int parseInput(char *input, char splitWords[][500], int maxWords)
{
    int numWords = 0;
    const char *delim = " ";
    char *nextWord = strtok(input, delim);

    while (nextWord != NULL && numWords < maxWords)
    {
        strcpy(splitWords[numWords], nextWord);
        nextWord = strtok(NULL, delim);
        numWords++;
    }

    return numWords;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_INPUT_CHARS 500
#define MAX_INPUT_WORDS 20

typedef struct _thread_data_t {
    const int *data;
    int startInd;
    int endInd;
    pthread_mutex_t *lock;
    long long int *totalSum;
} thread_data_t;

int readFile(char[], int[]);
void* arraySum(void*);

int main(int argc, char* argv[])
{
    return 0;
}

int readFile(char filename[], int arr[]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("File not found...\n");
        return -1;
    }
    
    int count = 0;
    while (fscanf(file, "%d", &arr[count]) == 1)
    {
        count++;
    }

    fclose(file);
    return count;
}
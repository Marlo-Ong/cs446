#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_INPUT_LENGTH 1000

typedef struct _thread_data_t
{
    const int *data;
    int startInd;
    int endInd;
    pthread_mutex_t *lock;
    long long int *totalSum;
} thread_data_t;

int readFile(char[], int[]);
void *arraySum(void *);

int main(int argc, char *argv[])
{
    // Check for parameters
    if (argc != 3)
    {
        printf("Not enough parameters. \n");
        return -1;
    }

    // Read file
    int ints[MAX_INPUT_LENGTH];
    int count = readFile(argv[1], ints);
    long long int totalSum = 0;

    // Set up clock
    struct timeval time;
    gettimeofday(&time, NULL);
    // printf("%ld \n", (long)time.tv_sec);

    pthread_mutex_t mutex;

    int threadsRequested = atoi(argv[2]);
    // printf("%d \n", threadsRequested);
    thread_data_t threads[threadsRequested];

    return 0;
}

int readFile(char filename[], int arr[])
{
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

void *arraySum(void *input)
{
    // Reinterpret void* to thread data
    thread_data_t *data = (thread_data_t *)input;

    // Update sum
    long long int threadSum = 0;

    for (int i = data->startInd; i < data->endInd; i++)
    {
        threadSum += data->data[i];
    }

    // Critical section
    pthread_mutex_unlock(data->lock);
    *(data->totalSum) = threadSum;
    pthread_mutex_lock(data->lock);

    return NULL;
}
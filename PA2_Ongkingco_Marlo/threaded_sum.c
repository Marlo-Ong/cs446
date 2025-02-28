#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_INPUT_LENGTH 1000000

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

    // Check requested thread count
    int threadsRequested = atoi(argv[2]);
    if (threadsRequested > count)
    {
        printf("Too many threads requested \n");
        return -1;
    }

    // Start clock
    struct timeval startTime;
    gettimeofday(&startTime, NULL);

    // Initialize mutex
    long long int totalSum = 0;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    // Initialize thread data
    thread_data_t threadData[threadsRequested];
    int numValuesPerThread = count / threadsRequested;

    for (int i = 0; i < threadsRequested; i++)
    {
        // Set array slice to be used for this thread
        threadData[i].data = ints;
        threadData[i].startInd = i * numValuesPerThread;
        threadData[i].endInd = (i * numValuesPerThread) + (numValuesPerThread);

        // (Give any remaining values to last thread)
        if (i == threadsRequested - 1)
        {
            threadData[i].endInd = count;
        }

        // Point to lock and sum
        threadData[i].lock = &mutex;
        threadData[i].totalSum = &totalSum;
    }

    // Initialize pthread objects
    pthread_t threads[threadsRequested];

    for (int i = 0; i < threadsRequested; i++)
    {
        pthread_create(&threads[i], NULL, arraySum, (void *)&threadData[i]);
        pthread_join(threads[i], NULL);
    }

    // End clock
    struct timeval endTime;
    gettimeofday(&endTime, NULL);

    double duration;
    duration = (endTime.tv_sec - startTime.tv_sec) * 1000.0;    // sec to ms
    duration += (endTime.tv_usec - startTime.tv_usec) / 1000.0; // us to ms

    printf("Total sum: %lld \nExecution time: %fms", totalSum, duration);

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
    *(data->totalSum) += threadSum;
    pthread_mutex_lock(data->lock);

    return NULL;
}
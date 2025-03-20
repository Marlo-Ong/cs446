#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/mman.h>

#define MAX_INPUT_LENGTH 2000000

#define ANSI_COLOR_GRAY "\x1b[30m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"

#define ANSI_COLOR_RESET "\x1b[0m"

#define TERM_CLEAR() printf("\033[H\033[J")
#define TERM_GOTOXY(x, y) printf("\033[%d;%dH", (y), (x))

typedef struct _thread_data_t
{
    int localTid;
    const int *data;
    int numVals;
    pthread_mutex_t *lock;
    long long int *totalSum;
} thread_data_t;

void *arraySum(void *);

int main(int argc, char *argv[])
{
    // Check for parameters
    if (argc != 2)
    {
        printf("Not enough parameters. \n");
        return -1;
    }

    // Dynamically allocate fixed-size array of 2 million ints.
    int ints[MAX_INPUT_LENGTH];

    // Check requested thread count
    int threadsRequested = atoi(argv[1]);

    // Initialize mutex
    long long int totalSum = 0;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    // Initialize thread data
    thread_data_t threadData[threadsRequested];

    for (int i = 0; i < threadsRequested; i++)
    {
        // Set array slice to be used for this thread
        threadData[i].localTid = i;
        threadData[i].data = ints;
        threadData[i].numVals = MAX_INPUT_LENGTH;

        // Point to lock and sum
        threadData[i].lock = &mutex;
        threadData[i].totalSum = &totalSum;
    }

    // Initialize pthread objects
    pthread_t threads[threadsRequested];

    for (int i = 0; i < threadsRequested; i++)
    {
        pthread_create(&threads[i], NULL, arraySum, (void *)&threadData[i]);
    }
    for (int i = 0; i < threadsRequested; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

void *arraySum(void *input)
{
    // Reinterpret void* to thread data
    thread_data_t *data = (thread_data_t *)input;

    // Update sum
    long long int threadSum = 0;

    // Run loop sum indefinitely
    while (1)
    {
        long latency_max = 0;

        for (int i = 0; i < data->numVals; i++)
        {
            struct timespec start;
            clock_gettime(CLOCK_REALTIME, &start);

            threadSum += data->data[i];

            struct timespec end;
            clock_gettime(CLOCK_REALTIME, &end);

            // Update max latency
            long latency = end.tv_nsec - start.tv_nsec;
            if (latency > latency_max)
                latency_max = latency;
        }

        // Critical section
        pthread_mutex_unlock(data->lock);
        *(data->totalSum) += threadSum;
        pthread_mutex_lock(data->lock);

        printf("\nMax latency: %ld", latency_max);
    }

    return NULL;
}

void print_progress(pid_t localTid, size_t value)
{
    pid_t tid = syscall(__NR_gettid);

    TERM_GOTOXY(0, localTid + 1);

    char prefix[256];
    size_t bound = 100;
    sprintf(prefix, "%d: %ld (ns) \t[", tid, value);
    const char suffix[] = "]";
    const size_t prefix_length = strlen(prefix);
    const size_t suffix_length = sizeof(suffix) - 1;
    char *buffer = calloc(bound + prefix_length + suffix_length + 1, 1);
    size_t i = 0;

    strcpy(buffer, prefix);
    for (; i < bound; ++i)
    {
        buffer[prefix_length + i] = i < value / 10000 ? '#' : ' ';
    }
    strcpy(&buffer[prefix_length + i], suffix);

    if (!(localTid % 7))
        printf(ANSI_COLOR_WHITE "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);
    else if (!(localTid % 6))
        printf(ANSI_COLOR_BLUE "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);
    else if (!(localTid % 5))
        printf(ANSI_COLOR_RED "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);
    else if (!(localTid % 4))
        printf(ANSI_COLOR_GREEN "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);
    else if (!(localTid % 3))
        printf(ANSI_COLOR_CYAN "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);
    else if (!(localTid % 2))
        printf(ANSI_COLOR_YELLOW "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);
    else if (!(localTid % 1))
        printf(ANSI_COLOR_MAGENTA "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);
    else
        printf("\b%c[2K\r%s\n", 27, buffer);

    fflush(stdout);
    free(buffer);
}
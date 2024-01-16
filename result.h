#ifndef RESULT_H
#define RESULT_H

#include "task.h"
#include <pthread.h>

#define MAX_RESULTS 1024 * 1024 // Define the maximum number of results

typedef struct
{
    unsigned char *encoded_data; // Pointer to store the encoded data
    size_t encoded_size;         // Size of the encoded data
    int Order;
} Result;

typedef struct
{
    Result results[MAX_RESULTS];
    pthread_mutex_t mutex;
    pthread_cond_t result_available; // New condition variable
    int processed_count;
} ResultQueue;

void result_queue_init(ResultQueue *rq);
void result_queue_push(ResultQueue *rq, Result *result);
void result_queue_destroy(ResultQueue *rq);

#endif // RESULT_H
#include "result.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void result_queue_init(ResultQueue *rq)
{
    memset(rq->results, 0, sizeof(rq->results));
    pthread_mutex_init(&rq->mutex, NULL);
    pthread_cond_init(&rq->result_available, NULL); // Initialize condition variable
    rq->processed_count = 0;
}

void result_queue_destroy(ResultQueue *rq)
{
    // Clean up the allocated data for each result
    for (int i = 0; i < MAX_RESULTS; ++i)
    {
        free(rq->results[i].encoded_data);
        rq->results[i].encoded_data = NULL;
    }
    pthread_mutex_destroy(&rq->mutex);
    pthread_cond_destroy(&rq->result_available);
}

// Push a result onto the result queue
void result_queue_push(ResultQueue *rq, Result *result)
{
    pthread_mutex_lock(&rq->mutex);
    if (result->Order >= MAX_RESULTS)
    {
        // Handle error: result order is out of bounds
        fprintf(stderr, "Error: Result order %d is out of bounds.\n", result->Order);
    }
    else
    {
        if (rq->results[result->Order].encoded_data != NULL)
        {
            // Handle error: result for this order is already present
            fprintf(stderr, "Error: Result for order %d is already present.\n", result->Order);
        }
        else
        {
            // Copy the result into the results array at the position corresponding to its order
            rq->results[result->Order] = *result;
            rq->processed_count++;
        }
    }
    // signal there is resource avaliable
    pthread_cond_signal(&rq->result_available);
    pthread_mutex_unlock(&rq->mutex);
}
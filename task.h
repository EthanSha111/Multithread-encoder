#ifndef TASK_H
#define TASK_H

#include <pthread.h>
#include <stdbool.h>

#define CHUNK_SIZE 4096 // Size of each chunk
#define UCHAR_MAX 256
#define MAX_TASKS 10000
extern int global_task_order; // To generate unique order for each task

typedef struct
{
    unsigned char data[CHUNK_SIZE]; // Data to encode
    size_t size;                    // Actual data size
    int Order;                      // Order of the task

} Task;

typedef struct TaskNode
{
    Task task;
    struct TaskNode *next;
} TaskNode;

typedef struct
{
    TaskNode *head;
    TaskNode *tail;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    bool all_tasks_processed;
    pthread_mutex_t all_tasks_processed_mutex;

} TaskQueue;

void task_queue_init(TaskQueue *tq);
int task_queue_push(TaskQueue *tq, Task *task);
int task_queue_pop(TaskQueue *tq, Task *task);
void task_queue_destroy(TaskQueue *tq);

#endif // TASK_H
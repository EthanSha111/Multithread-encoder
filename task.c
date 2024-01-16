#include "task.h"
#include <stdio.h>
#include <stdlib.h>

int global_task_order = 0; // Initialize global task order
int pop_int = 0;

void task_queue_init(TaskQueue *tq)
{
    tq->head = NULL;
    tq->tail = NULL;
    pthread_mutex_init(&tq->mutex, NULL);
    pthread_cond_init(&tq->not_empty, NULL);
    tq->all_tasks_processed = false;
    pthread_mutex_init(&tq->all_tasks_processed_mutex, NULL);
}
int task_queue_push(TaskQueue *tq, Task *task)
{
    pthread_mutex_lock(&tq->mutex);

    TaskNode *new_node = malloc(sizeof(TaskNode));
    if (new_node == NULL)
    {
        pthread_mutex_unlock(&tq->mutex);
        return -1; // Failed to allocate memory for the new task node
    }
    // set unique order num for task
    task->Order = global_task_order++;
    new_node->task = *task;
    new_node->next = NULL;

    if (tq->tail == NULL)
    { // If the queue was empty
        tq->head = tq->tail = new_node;
        printf("The first node is pushing");
    }
    else
    {
        tq->tail->next = new_node;
        tq->tail = new_node;
    }

    pthread_cond_signal(&tq->not_empty);
    pthread_mutex_unlock(&tq->mutex);

    return 0;
}

int task_queue_pop(TaskQueue *tq, Task *task)
{
    pthread_mutex_lock(&tq->mutex);
    int pop = pop_int++;

    while (tq->head == NULL)
    {
        // Lock the mutex for all_tasks_processed
        pthread_mutex_lock(&tq->all_tasks_processed_mutex);

        // Check if all tasks are processed and no more tasks will be added
        if (tq->all_tasks_processed)
        {
            pthread_mutex_unlock(&tq->all_tasks_processed_mutex);
            pthread_mutex_unlock(&tq->mutex);
            return -1; // Indicate that there are no more tasks
        }

        // Unlock the mutex after checking the condition
        pthread_mutex_unlock(&tq->all_tasks_processed_mutex);

        // Wait for new tasks to be added if the queue is empty
        pthread_cond_wait(&tq->not_empty, &tq->mutex);
    }
    // fprintf(stderr, "the popped number is %d\n", pop);
    // Pop the task from the queue
    TaskNode *temp_node = tq->head;
    *task = temp_node->task;
    tq->head = tq->head->next;
    if (tq->head == NULL)
    {
        tq->tail = NULL;
    }

    free(temp_node);
    pthread_mutex_unlock(&tq->mutex);

    return 0; // Success
}

void task_queue_destroy(TaskQueue *tq)
{
    // Free all remaining nodes in the queue
    TaskNode *current = tq->head;
    while (current != NULL)
    {
        TaskNode *temp = current;
        current = current->next;
        free(temp);
    }

    pthread_mutex_destroy(&tq->mutex);
    pthread_cond_destroy(&tq->not_empty);
    // pthread_mutex_destroy(&tq->all_tasks_processed_mutex);
}
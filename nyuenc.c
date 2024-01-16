#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>    // For open
#include <unistd.h>   // For close
#include <sys/mman.h> // For mmap
#include <sys/stat.h> // For fstat
#include "task.h"
#include "result.h"

ResultQueue result_queue;
static int total_task = 0;
static unsigned char last_char = 0;
static unsigned char last_count = 0;

void encode_rle(const unsigned char *data, size_t size, Result *result, int Res_order)
{
    size_t buffer_index = 0;
    size_t buffer_capacity = size * 2;
    result->encoded_data = malloc(buffer_capacity);
    if (!result->encoded_data)
    {
        perror("Memory allocation failed for RLE encoding buffer");
        exit(EXIT_FAILURE);
    }

    unsigned char last_char = data[0];
    unsigned char run_length = 1;

    for (size_t i = 1; i < size; ++i)
    {
        if (data[i] == last_char && run_length < UCHAR_MAX)
        {
            run_length++;
        }
        else
        {
            result->encoded_data[buffer_index++] = last_char;
            result->encoded_data[buffer_index++] = run_length;
            last_char = data[i];
            run_length = 1;
        }
    }

    
    result->encoded_data[buffer_index++] = last_char;
    result->encoded_data[buffer_index++] = run_length;

   
    result->encoded_data = realloc(result->encoded_data, buffer_index);
    result->encoded_size = buffer_index;
    result->Order = Res_order;
}



void *worker_thread_function(void *arg)
{
    TaskQueue *tq = (TaskQueue *)arg;
    while (1)
    {
        Task task;
        if (task_queue_pop(tq, &task) != 0)
        {
            break;
        }

        Result *result = malloc(sizeof(Result));
        if (result == NULL)
        {
            perror("Failed to allocate memory for result");
            exit(EXIT_FAILURE);
        }

       

        encode_rle(task.data, task.size, result, task.Order);



        result_queue_push(&result_queue, result);
    }
    return NULL;
}

void combine_result(Result *result, FILE *output)
{
    for (size_t j = 0; j < result->encoded_size; j += 2)
    {
        unsigned char current_char = result->encoded_data[j];
        unsigned char current_count = result->encoded_data[j + 1];

        if (current_char == last_char)
        {
            
            last_count += current_count;
        }
        else
        {
            if (last_count > 0)
            {
                
                fwrite(&last_char, 1, 1, output);
                fwrite(&last_count, 1, 1, output);
            }
            
            last_char = current_char;
            last_count = current_count;
        }
    }
}

int main(int argc, char *argv[])
{

    int num_workers = 1;
    int arg_offset = 1;

    if (argc > 1 && strcmp(argv[1], "-j") == 0)
    {
        if (argc < 4)
        {
            fprintf(stderr, "Usage: %s -j <num_workers> <file1> [file2 ...] [output_file]\n", argv[0]);
            return EXIT_FAILURE;
        }
        num_workers = atoi(argv[2]);
        arg_offset = 3;
    }

    
    TaskQueue task_queue;
    task_queue_init(&task_queue);
    result_queue_init(&result_queue);
    pthread_t workers[num_workers];

    
    for (int i = 0; i < num_workers; i++)
    {
        if (pthread_create(&workers[i], NULL, worker_thread_function, (void *)&task_queue) != 0)
        {
            perror("Failed to create worker thread");
            return EXIT_FAILURE;
        }
    }
    unsigned char buffer[CHUNK_SIZE];
    int buffer_len = 0;

    for (int i = arg_offset; i < argc; i++)
    {
        fprintf(stderr, "Processing file: %s\n", argv[i]);
        FILE *file = fopen(argv[i], "rb");
        if (!file)
        {
            perror("Cannot open file");
            return EXIT_FAILURE;
        }

        size_t bytes_read;
        while ((bytes_read = fread(buffer + buffer_len, 1, CHUNK_SIZE - buffer_len, file)) > 0)
        {
            buffer_len += bytes_read;
            if (buffer_len == CHUNK_SIZE)
            {
                Task task;
                memcpy(task.data, buffer, CHUNK_SIZE);
                task.size = CHUNK_SIZE;
                task_queue_push(&task_queue, &task);
                total_task++;
                buffer_len = 0;
            }
        }

        fclose(file);
    }

    
    if (buffer_len > 0)
    {
        Task task;
        memcpy(task.data, buffer, buffer_len);
        task.size = buffer_len;
        task_queue_push(&task_queue, &task);
        total_task++;
    }

   
    pthread_mutex_lock(&task_queue.all_tasks_processed_mutex);
    task_queue.all_tasks_processed = true;
    pthread_mutex_unlock(&task_queue.all_tasks_processed_mutex);

    pthread_mutex_lock(&task_queue.mutex);
    pthread_cond_broadcast(&task_queue.not_empty);
    pthread_mutex_unlock(&task_queue.mutex);


    FILE *output = fdopen(fileno(stdout), "wb");
    if (output == NULL)
    {
        perror("Failed to open stdout for writing");
        return EXIT_FAILURE;
    }
    for (int order = 0; order < total_task; ++order)
    {
        pthread_mutex_lock(&result_queue.mutex);
        while (result_queue.results[order].encoded_data == NULL)
        {
            pthread_cond_wait(&result_queue.result_available, &result_queue.mutex);
        }
        combine_result(&result_queue.results[order], output);
        pthread_mutex_unlock(&result_queue.mutex);
    }
    if (last_count > 0)
    {
        fwrite(&last_char, 1, 1, output);
        fwrite(&last_count, 1, 1, output);
    }
    for (int i = 0; i < num_workers; i++)
    {
        pthread_join(workers[i], NULL);
    }
    fclose(output);

    task_queue_destroy(&task_queue);
    result_queue_destroy(&result_queue);

    return EXIT_SUCCESS;
}

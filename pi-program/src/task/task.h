#ifndef TASK_H
#define TASK_H

#include <pico/util/queue.h>

#ifndef MAX_PERSISTENT_TASKS
#define MAX_PERSISTENT_TASKS 4
#endif

typedef struct task_t
{
    void *(*routine)(void *);
    void *data;
} task_t;

typedef struct task_scheduler_t
{
    queue_t once_queue;
    task_t persistent_tasks[MAX_PERSISTENT_TASKS];
} task_scheduler_t;

int task_scheduler_init (task_scheduler_t *scheduler);

int task_scheduler_deinit (task_scheduler_t *scheduler);

#endif /** TASK_H */
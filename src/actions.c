#include "codexion.h"

void    log_action(t_coder *coder, char *message)
{
    long long   timestamp;
    
    pthread_mutex_lock(&coder->table->write_lock);
    pthread_mutex_lock(&coder->table->stop_lock);
    
    if (!coder->table->stop_sim)
    {
        timestamp = get_time_in_ms() - coder->table->start_time;
        printf("%lld %d %s\n", timestamp, coder->id, message);
    }

    pthread_mutex_unlock(&coder->table->stop_lock);
    pthread_mutex_unlock(&coder->table->write_lock);
}

void    think_action(t_coder *coder)
{
    log_action(coder, "is thinking");
}

void    refactor_action(t_coder *coder)
{
    log_action(coder, "is refactoring");
    ft_usleep(coder->table->time_to_refactor, coder->table);
}

static int  grab_dongle(t_coder *coder, int dongle_id)
{
    t_heap      *heap;
    long long   priority;

    heap = &coder->table->dongle_queues[dongle_id];
    priority = get_priority(coder);
    heap_push(heap, coder->id, priority);

    while (1)
    {
        pthread_mutex_lock(&coder->table->stop_lock);
        if (coder->table->stop_sim)
        {
            pthread_mutex_unlock(&coder->table->stop_lock);
            heap_pop(heap);
            return (0);
        }
        pthread_mutex_unlock(&coder->table->stop_lock);
        if (is_coder_next(heap, coder->id))
        {
            pthread_mutex_lock(&coder->table->dongle_locks[dongle_id]);
            return (1);
        }
        usleep(100);
    }
}

int compile_action(t_coder *coder)
{
    if (!grab_dongle(coder, coder->left_dongle))
        return (0);
    log_action(coder, "has taken a dongle");

    if (!grab_dongle(coder, coder->right_dongle))
    {
        pthread_mutex_unlock(&coder->table->dongle_locks[coder->left_dongle]);
        heap_pop(&coder->table->dongle_queues[coder->left_dongle]);
        return (0);
    }
    log_action(coder, "has taken a dongle");

    log_action(coder, "is compiling");
    pthread_mutex_lock(&coder->table->stop_lock);
    coder->last_compile = get_time_in_ms();
    pthread_mutex_unlock(&coder->table->stop_lock);
    ft_usleep(coder->table->time_to_compile, coder->table);
    coder->compiles_done++;

    pthread_mutex_unlock(&coder->table->dongle_locks[coder->left_dongle]);
    heap_pop(&coder->table->dongle_queues[coder->left_dongle]);

    pthread_mutex_unlock(&coder->table->dongle_locks[coder->right_dongle]);
    heap_pop(&coder->table->dongle_queues[coder->right_dongle]);

    return (1);
}
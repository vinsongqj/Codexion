#include "codexion.h"

void    log_action(t_coder *coder, char *message)
{
    long long   timestamp;
    
    timestamp = get_time_in_ms() - coder->table->start_time;
    pthread_mutex_lock(&coder->table->write_lock);
    pthread_mutex_lock(&coder->table->stop_lock);
    
    if (!coder->table->stop_sim)
        printf("%lld %d %s\n", timestamp, coder->id, message);

    pthread_mutex_unlock(&coder->table->stop_lock);
    pthread_mutex_unlock(&coder->table->write_lock);
}

void    debug_action(t_coder *coder)
{
    log_action(coder, "is debugging");
}

void    refactor_action(t_coder *coder)
{
    log_action(coder, "is refactoring");
    ft_usleep(coder->table->time_to_refactor, coder->table);
}

static int  grab_dongle(t_coder *coder, int dongle_id)
{
    t_heap      *heap;
    int         i;

    heap = &coder->table->dongle_queues[dongle_id];
    heap_push(heap, coder->id, get_priority(coder));
    i = 0;

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
            heap_pop(heap);
            return (1);
        }
        if (++i >= 10)
        {
            heap_pop(heap);
            heap_push(heap, coder->id, get_priority(coder));
            i = 0;
        }
        usleep(50);
    }
}

int compile_action(t_coder *coder)
{
    int first;
    int second;

    if (coder->id % 2 == 0)
    {
        first = coder->right_dongle;
        second = coder->left_dongle;
    }
    else
    {
        first = coder->left_dongle;
        second = coder->right_dongle;
    }
    if (!grab_dongle(coder, first))
        return (0);
    log_action(coder, "has taken a dongle");

    if (coder->table->number_of_coders == 1)
    {
        while (1)
        {
            pthread_mutex_lock(&coder->table->stop_lock);
            if (coder->table->stop_sim)
            {
                pthread_mutex_unlock(&coder->table->stop_lock);
                break;
            }
            pthread_mutex_unlock(&coder->table->stop_lock);
            usleep(500);
        }
        pthread_mutex_unlock(&coder->table->dongle_locks[first]);
        heap_pop(&coder->table->dongle_queues[first]);
        return (0);
    }
    if (!grab_dongle(coder, second))
    {
        pthread_mutex_unlock(&coder->table->dongle_locks[first]);
        heap_pop(&coder->table->dongle_queues[first]);
        return (0);
    }
    log_action(coder, "has taken a dongle");

    pthread_mutex_lock(&coder->table->stop_lock);
    coder->last_compile = get_time_in_ms();
    pthread_mutex_unlock(&coder->table->stop_lock);
    log_action(coder, "is compiling");
    ft_usleep(coder->table->time_to_compile, coder->table);
    coder->compiles_done++;

    pthread_mutex_unlock(&coder->table->dongle_locks[first]);
    pthread_mutex_unlock(&coder->table->dongle_locks[second]);

    return (1);
}
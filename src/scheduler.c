#include "codexion.h"

long long   get_priority(t_coder *coder)
{
    long long   priority;

    if (coder->table->scheduler == 0)
        return (get_time_in_ms());

    pthread_mutex_lock(&coder->table->stop_lock);
    priority = coder->last_compile + coder->table->time_to_burnout;
    pthread_mutex_unlock(&coder->table->stop_lock);
    return (priority);
}

static void    swap_nodes(t_node *a, t_node *b)
{
    t_node temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

void    heap_push(t_heap *heap, int coder_id, long long priority)
{
    int i;

    pthread_mutex_lock(&heap->lock);
    i = heap->size;
    heap->nodes[i].coder_id = coder_id;
    heap->nodes[i].priority = priority;
    heap->size++;

    while (i > 0 && heap->nodes[i].priority < heap->nodes[(i - 1) / 2].priority)
    {
        swap_nodes(&heap->nodes[i], &heap->nodes[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
    
    pthread_mutex_unlock(&heap->lock);
}

void    heap_pop(t_heap *heap)
{
    int i;
    int smallest;
    int left;
    int right;

    pthread_mutex_lock(&heap->lock);
    if (heap->size == 0)
    {
        pthread_mutex_unlock(&heap->lock);
        return;
    }
    heap->nodes[0] = heap->nodes[heap->size - 1];
    heap->size--;

    i = 0;
    while (1)
    {
        smallest = i;
        left = 2 * i + 1;
        right = 2 * i + 2;

        if (left < heap->size && heap->nodes[left].priority < heap->nodes[smallest].priority)
            smallest = left;
        if (right < heap->size && heap->nodes[right].priority < heap->nodes[smallest].priority)
            smallest = right;
        if (smallest != i)
        {
            swap_nodes(&heap->nodes[i], &heap->nodes[smallest]);
            i = smallest;
        }
        else
            break;
    }
    pthread_mutex_unlock(&heap->lock);
}

int is_coder_next(t_heap *heap, int coder_id)
{
    int result;

    result = 0;
    pthread_mutex_lock(&heap->lock);
    if (heap->size > 0 && heap->nodes[0].coder_id == coder_id)
        result = 1;
    pthread_mutex_unlock(&heap->lock);

    return (result);
}
#include "codexion.h"

void    *coder_routine(void *arg)
{
    t_coder *coder;
    t_table *table;

    coder = (t_coder *)arg;
    table = coder->table;

    if (coder->id % 2 != 0)
        ft_usleep(table->time_to_compile / 2, table);

    while (1)
    {
        pthread_mutex_lock(&table->stop_lock);
        if (table->stop_sim)
        {
            pthread_mutex_unlock(&table->stop_lock);
            break;
        }
        pthread_mutex_unlock(&table->stop_lock);

        debug_action(coder);
        if (!compile_action(coder))
            break;
        
        refactor_action(coder);

        pthread_mutex_lock(&table->stop_lock);
        if (table->number_of_compiles_required != -1 &&
            coder->compiles_done >= table->number_of_compiles_required)
        {
            pthread_mutex_unlock(&table->stop_lock);
            break;
        }
        pthread_mutex_unlock(&table->stop_lock);
    }
    return (NULL);
}

void    *monitor_routine(void *arg)
{
    t_table *table;
    int     i;
    int     finished_count;
    int     is_done;

    table = (t_table *)arg;

    while (1)
    {
        i = 0;
        finished_count = 0;

        while (i < table->number_of_coders)
        {
            pthread_mutex_lock(&table->stop_lock);
            is_done = (table->number_of_compiles_required != -1 &&
                       table->coders[i].compiles_done >= table->number_of_compiles_required);
            if (!is_done && (get_time_in_ms() - table->coders[i].last_compile >= table->time_to_burnout))
            {
                table->stop_sim = 1;
                pthread_mutex_lock(&table->write_lock);
                printf("%lld %d burned out\n",
                        get_time_in_ms() - table->start_time, table->coders[i].id);
                pthread_mutex_unlock(&table->write_lock);
                pthread_mutex_unlock(&table->stop_lock);
                return (NULL);
            }
            if (is_done)
                finished_count++;
            pthread_mutex_unlock(&table->stop_lock);
            i++;
        }
        if (table->number_of_compiles_required != -1 && finished_count == table->number_of_coders)
        {
            pthread_mutex_lock(&table->stop_lock);
            table->all_finished = 1;
            table->stop_sim = 1;
            pthread_mutex_unlock(&table->stop_lock);
            usleep(5000);
            return (NULL);
        }
    usleep(1000);
    }
return (NULL);
}
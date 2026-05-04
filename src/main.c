#include "codexion.h"

int main(int argc, char **argv)
{
    t_table table;
    int     i;

    if (argc != 9)
    {
        printf("Error: Wrong number of arguments.\n");
        return (1);
    }
    
    table.start_time = get_time_in_ms();
    table.number_of_coders = atoi(argv[1]);
    table.time_to_burnout = ft_atoll(argv[2]);
    table.time_to_compile = ft_atoll(argv[3]);
    table.time_to_debug = ft_atoll(argv[4]);
    table.time_to_refactor = ft_atoll(argv[5]);
    table.number_of_compiles_required = atoi(argv[6]);
    table.dongle_cooldown = ft_atoll(argv[7]);
    table.stop_sim = 0;
    
    if (strcmp(argv[8], "fifo") == 0)
        table.scheduler = 0;
    else if (strcmp(argv[8], "edf") == 0)
        table.scheduler = 1;
    else
        return (1);

    table.dongle_locks = malloc(sizeof(pthread_mutex_t) * table.number_of_coders);
    table.coders = malloc(sizeof(t_coder) * table.number_of_coders);
    if (!table.dongle_locks || !table.coders)
        return (1);

    i = 0;
    while (i < table.number_of_coders)
    {
        pthread_mutex_init(&table.dongle_locks[i], NULL);
        table.coders[i].id = i + 1;
        table.coders[i].table = &table;
        table.coders[i].compiles_done = 0;
        table.coders[i].last_compile = table.start_time;
        table.coders[i].left_dongle = i;
        table.coders[i].right_dongle = (i + 1) % table.number_of_coders;

        i++;
    }
    pthread_mutex_init(&table.write_lock, NULL);
    pthread_mutex_init(&table.stop_lock, NULL);

    i = 0;
    while (i < table.number_of_coders)
    {
        pthread_create(&table.coders[i].thread_id, NULL, &coder_routine, &table.coders[i]);
        i++;
    }

    i = 0;
    while (i < table.number_of_coders)
    {
        pthread_join(table.coders[i].thread_id, NULL);
        i++;
    }

    i = 0;
    while (i < table.number_of_coders)
    {
        pthread_mutex_destroy(&table.dongle_locks[i]);
        i++;
    }
    pthread_mutex_destroy(&table.write_lock);
    pthread_mutex_destroy(&table.stop_lock);
    free(table.dongle_locks);
    free(table.coders);

    return (0);
}
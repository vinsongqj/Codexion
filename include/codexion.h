#ifndef CODEXION_H
#define CODEXION_H

#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_table{
    int             number_of_coders;
    int             time_to_burnout;
    int             time_to_compile;
    int             time_to_debug;
    int             time_to_refactor;
    int             number_of_compiles_required;
    int             dongle_cooldown;
    int             scheduler;
    int             stop_sim;
    pthread_mutex_t stop_lock;
    pthread_mutex_t write_lock;
    pthread_mutex_t *dongle_locks;
}              t_table;

typedef struct s_coder{
    int         id;
    long long   last_compile;
    int         compiles_done;
    int         left_dongle;
    int         right_dongle;
    t_table     *table;
    pthread_t   thread_id;
}              t_coder;

#endif
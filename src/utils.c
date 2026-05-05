#include "codexion.h"

long long   ft_atoll(const char *str)
{
    long long   res;
    int         sign;
    int         i;

    res = 0;
    sign = 1;
    i = 0;

    while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
        i++;
    if (str[i] == '-' || str[i] == '+')
    {
        if (str[i] == '-')
            sign = -1;
        i++;
    }
    while (str[i] >= '0' && str[i] <= '9')
    {
        res = (res * 10) + (str[i] - '0');
        i++;
    }
    return (res * sign);
}

long long   get_time_in_ms(void)
{
    struct timeval  tv;
    
    if (gettimeofday(&tv, NULL) == -1)
        return (0);
    return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

void    ft_usleep(long long time_in_ms, t_table *table)
{
    long long   start_time;

    start_time = get_time_in_ms();
    while ((get_time_in_ms() - start_time) < time_in_ms)
    {
        pthread_mutex_lock(&table->stop_lock);
        if (table->stop_sim)
        {
            pthread_mutex_unlock(&table->stop_lock);
            break;
        }
        pthread_mutex_unlock(&table->stop_lock);
        usleep(200);
    }
}
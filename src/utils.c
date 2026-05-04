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

long long   get_time_in_ms(void);
{
    struct timeval  tv;
    
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}
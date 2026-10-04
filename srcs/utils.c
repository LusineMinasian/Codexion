#include "codexion.h"

long    time_now_ms(t_sim *sim)
{
    struct timeval  now;

    gettimeofday(&now, NULL);
    return ((now.tv_sec - sim->t0.tv_sec) * 1000 + (now.tv_usec - sim->t0.tv_usec) / 1000);
}

int sim_should_stop(t_sim *sim)
{
    int stopped;

    pthread_mutex_lock(&sim->stop_mutex);
    stopped = sim->stop;
    pthread_mutex_unlock(&sim->stop_mutex);
    return (stopped);
}

void sim_request_stop(t_sim *sim)
{
    pthread_mutex_lock(&sim->stop_mutex);
    sim->stop = 1;
    pthread_mutex_unlock(&sim->stop_mutex);
}

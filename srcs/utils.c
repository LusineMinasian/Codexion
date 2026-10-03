#include "codexion.h"

long    time_now_ms(t_sim *sim)
{
    struct timeval  now;

    gettimeofday(&now, NULL);
    return ((now.tv_sec - sim->t0.tv_sec) * 1000 + (now.tv_usec - sim->t0.tv_usec) / 1000);
}

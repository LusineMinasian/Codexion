#include "codexion.h"

void    log_event(t_sim *sim, int coder_id, const char *msg)
{
    long  ts;

    pthread_mutex_lock(&sim->log_mutex);
    ts = time_now_ms(sim);
    printf("%ld %d %s\n", ts, coder_id, msg);
    pthread_mutex_unlock(&sim->log_mutex);
}

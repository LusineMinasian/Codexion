#include "codexion.h"

int	dongles_init(t_sim *sim)
{
    int i;

    sim->dongles = malloc(sizeof(t_dongle) * sim->number_of_coders);
    if (!sim->dongles)
    {
        fprintf(stderr, "codexion: error\n");
        return (0);
    }
    i = 0;
    while (i < sim->number_of_coders)
    {
        sim->dongles[i].id = i;
        sim->dongles[i].taken = 0;
        sim->dongles[i].available_at = 0;
        sim->dongles[i].sim = sim;
        pthread_mutex_init(&sim->dongles[i].mutex, NULL);
        pthread_cond_init(&sim->dongles[i].cond, NULL);
        heap_init(&sim->dongles[i].waiters);
        i++;
    }
    return (1);
}
#include "codexion.h"

int	coders_init(t_sim *sim)
{
    int i;

    sim->coders = malloc(sizeof(t_coder) * sim->number_of_coders);
    if (!sim->coders)
    {
        fprintf(stderr, "codexion: error\n");
        return (0);
    }
    i = 0;
    while (i < sim->number_of_coders)
    {
        sim->coders[i].id = i + 1;
        sim->coders[i].left = sim->coders[i].id - 1;
        sim->coders[i].right = sim->coders[i].id % sim->number_of_coders;
        sim->coders[i].compile_count = 0;
        sim->coders[i].last_compile_start = 0;
        sim->coders[i].sim = sim;
        pthread_mutex_init(&sim->coders[i].mutex, NULL);
        i++;
    }
    return (1);
}
#ifndef CODEXION_H
# define CODEXION_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum e_scheduler
{
    SCHED_FIFO,
    SCHED_EDF
}   t_scheduler;

typedef struct s_sim
{
    int     	number_of_coders;
	long		time_to_burnout;
	long		time_to_compile;
	long		time_to_debug;
	long		time_to_refactor;
	int			number_of_compiles_required;
	long		dongle_cooldown;
	t_scheduler	scheduler;
}	t_sim;

typedef struct s_heap_node
{
    long    key;
    long    tiebreak;
    int     coder_id;
}   t_heap_node;

typedef struct s_heap
{
    t_heap_node    *nodes;
    int             size;
    int             capacity;
}   t_heap;

int parse_args(int argc, char **argv, t_sim *sim);
int		heap_init(t_heap *h);
int		heap_push(t_heap *h, long key, long tiebreak, int coder_id);
int		heap_pop_min(t_heap *h, t_heap_node *out);
int		heap_peek_min(t_heap *h, t_heap_node *out);
int		heap_remove_coder(t_heap *h, int coder_id);
void	heap_destroy(t_heap *h);

#endif
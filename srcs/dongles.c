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
        sim->dongles[i].sim = 0;
        pthread_mutex_init(&sim->dongles[i].mutex, NULL);
        pthread_cond_init(&sim->dongles[i].cond, NULL);
        heap_init(&sim->dongles[i].waiters);
        i++;
    }
    return (1);
}

void dongles_destroy(t_sim *sim)
{
    int i;

    i = 0;
    while (i < sim->number_of_coders)
    {
        pthread_mutex_destroy(&sim->dongles[i].mutex);
        pthread_cond_destroy(&sim->dongles[i].cond);
        heap_destroy(&sim->dongles[i].waiters);
        i++;
    }
    free(sim->dongles);
    sim->dongles = NULL;
}

void    dongle_release(t_dongle *d)
{
    pthread_mutex_lock(&d->mutex);
    d->taken = 0;
    d->available_at = time_now_ms(d->sim) + d->sim->dongle_cooldown;
    pthread_cond_broadcast(&d->cond);
    pthread_mutex_unlock(&d->mutex);
}

static void ms_to_timespec(t_sim *sim, long ms, struct timespec *ts)
{
    long    usec;

    usec = sim->t0.tv_usec + (ms % 1000) * 1000;
    ts->tv_sec = sim->t0.tv_sec + ms / 1000;
    ts->tv_nsec = usec * 1000;
    if (ts->tv_nsec >= 1000000000)
    {
        ts->tv_sec = ts->tv_sec + 1;
        ts->tv_nsec = ts->tv_nsec - 1000000000;
    }
}

int	dongle_acquire(t_dongle *d, t_coder *c)
{
	long		    deadline;
	long			seq;
	long			key;
	t_heap_node		top;
	struct timespec	ts;

	pthread_mutex_lock(&c->mutex);
	deadline = c->last_compile_start + d->sim->time_to_burnout;
	pthread_mutex_unlock(&c->mutex);
	pthread_mutex_lock(&d->mutex);
	seq = d->next_seq;
	d->next_seq = d->next_seq + 1;
	if (d->sim->scheduler == SCHEDULER_FIFO)
		key = seq;
	else
		key = deadline;
	if (!heap_push(&d->waiters, key, seq, c->id))
	{
		pthread_mutex_unlock(&d->mutex);
		return (0);
	}
	while (1)
	{
		if (sim_should_stop(d->sim))
		{
			heap_remove_coder(&d->waiters, c->id);
			pthread_mutex_unlock(&d->mutex);
			return (0);
		}
		if (!d->taken && heap_peek_min(&d->waiters, &top)
			&& top.coder_id == c->id)
		{
			if (time_now_ms(d->sim) >= d->available_at)
				break ;
			ms_to_timespec(d->sim, d->available_at, &ts);
			pthread_cond_timedwait(&d->cond, &d->mutex, &ts);
		}
		else
			pthread_cond_wait(&d->cond, &d->mutex);
	}
	heap_pop_min(&d->waiters, &top);
	d->taken = 1;
	pthread_mutex_unlock(&d->mutex);
	return (1);
}
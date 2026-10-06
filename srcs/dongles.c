/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lminasia <lminasia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 15:43:48 by lminasia          #+#    #+#             */
/*   Updated: 2026/10/06 15:43:48 by lminasia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Both dongles get the same (key, ticket) from one global counter, so all
** queues share a single order: the best-ranked waiter is first in line for
** both of its dongles, and two coders can never block each other.
*/
static long	request_key(t_coder *c, long *ticket)
{
	t_sim	*sim;
	long	key;

	sim = c->sim;
	pthread_mutex_lock(&sim->ticket_mutex);
	*ticket = sim->next_ticket;
	sim->next_ticket = *ticket + 1;
	pthread_mutex_unlock(&sim->ticket_mutex);
	if (sim->scheduler == SCHEDULER_FIFO)
		return (*ticket);
	pthread_mutex_lock(&c->mutex);
	key = c->last_compile_start + sim->time_to_burnout;
	pthread_mutex_unlock(&c->mutex);
	return (key);
}

static int	pair_enqueue(t_dongle *a, t_dongle *b, t_coder *c)
{
	long	ticket;
	long	key;
	int		ok;

	key = request_key(c, &ticket);
	pthread_mutex_lock(&a->mutex);
	pthread_mutex_lock(&b->mutex);
	ok = heap_push(&a->waiters, key, ticket, c->id);
	if (ok && !heap_push(&b->waiters, key, ticket, c->id))
	{
		heap_remove_coder(&a->waiters, c->id);
		ok = 0;
	}
	pthread_mutex_unlock(&b->mutex);
	pthread_mutex_unlock(&a->mutex);
	return (ok);
}

/* Called with both mutexes held: free, cooled down, and first in line. */
static int	pair_ready(t_dongle *a, t_dongle *b, int coder_id)
{
	t_heap_node	top_a;
	t_heap_node	top_b;
	long		now;

	now = time_now_ms(a->sim);
	return (!a->taken && !b->taken
		&& now >= a->available_at && now >= b->available_at
		&& heap_peek_min(&a->waiters, &top_a) && top_a.coder_id == coder_id
		&& heap_peek_min(&b->waiters, &top_b) && top_b.coder_id == coder_id);
}

/* Called with both mutexes held: leaves both queues and unlocks. */
static int	pair_cancel(t_dongle *a, t_dongle *b, int coder_id)
{
	heap_remove_coder(&a->waiters, coder_id);
	heap_remove_coder(&b->waiters, coder_id);
	pthread_mutex_unlock(&b->mutex);
	pthread_mutex_unlock(&a->mutex);
	return (0);
}

/*
** Takes dongles a and b (a->id < b->id) together, never just one, so a
** coder never holds a dongle while waiting for the other. Changes to b do
** not signal a->cond, so the wait is capped at 1 ms and then re-checked.
*/
int	dongles_take(t_dongle *a, t_dongle *b, t_coder *c)
{
	t_heap_node		top;
	struct timespec	ts;

	if (!pair_enqueue(a, b, c))
		return (0);
	pthread_mutex_lock(&a->mutex);
	pthread_mutex_lock(&b->mutex);
	while (!pair_ready(a, b, c->id))
	{
		if (sim_should_stop(c->sim))
			return (pair_cancel(a, b, c->id));
		pthread_mutex_unlock(&b->mutex);
		ms_to_timespec(c->sim, time_now_ms(c->sim) + 1, &ts);
		pthread_cond_timedwait(&a->cond, &a->mutex, &ts);
		pthread_mutex_lock(&b->mutex);
	}
	heap_pop_min(&a->waiters, &top);
	heap_pop_min(&b->waiters, &top);
	a->taken = 1;
	b->taken = 1;
	pthread_mutex_unlock(&b->mutex);
	pthread_mutex_unlock(&a->mutex);
	return (1);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lminasia <lminasia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 15:43:48 by lminasia          #+#    #+#             */
/*   Updated: 2026/10/06 15:43:48 by lminasia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	time_now_ms(t_sim *sim)
{
	struct timeval	now;
	long			us;

	gettimeofday(&now, NULL);
	us = (now.tv_sec - sim->t0.tv_sec) * 1000000L
		+ (now.tv_usec - sim->t0.tv_usec);
	return (us / 1000);
}

int	sim_should_stop(t_sim *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->stop_mutex);
	stopped = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stopped);
}

void	sim_request_stop(t_sim *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
}

/* Sleeps ms milliseconds in short steps, returning early once stopped. */
void	sim_sleep(t_sim *sim, long ms)
{
	long	end;

	end = time_now_ms(sim) + ms;
	while (!sim_should_stop(sim) && time_now_ms(sim) < end)
		usleep(500);
}

void	ms_to_timespec(t_sim *sim, long ms, struct timespec *ts)
{
	long	usec;

	usec = sim->t0.tv_usec + (ms % 1000) * 1000;
	ts->tv_sec = sim->t0.tv_sec + ms / 1000;
	ts->tv_nsec = usec * 1000;
	if (ts->tv_nsec >= 1000000000)
	{
		ts->tv_sec = ts->tv_sec + 1;
		ts->tv_nsec = ts->tv_nsec - 1000000000;
	}
}

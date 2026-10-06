/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lminasia <lminasia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 15:43:48 by lminasia          #+#    #+#             */
/*   Updated: 2026/10/06 15:43:48 by lminasia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	coder_burned_out(t_sim *sim, t_coder *c)
{
	long	last;

	pthread_mutex_lock(&c->mutex);
	last = c->last_compile_start;
	pthread_mutex_unlock(&c->mutex);
	return (time_now_ms(sim) - last >= sim->time_to_burnout);
}

static int	all_finished(t_sim *sim)
{
	int	i;
	int	count;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_lock(&sim->coders[i].mutex);
		count = sim->coders[i].compile_count;
		pthread_mutex_unlock(&sim->coders[i].mutex);
		if (count < sim->number_of_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

void	wake_all_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

static int	find_burned_out(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (coder_burned_out(sim, &sim->coders[i]))
			return (sim->coders[i].id);
		i++;
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		id;

	sim = arg;
	while (1)
	{
		id = find_burned_out(sim);
		if (id)
			log_burnout(sim, id);
		else if (all_finished(sim))
			sim_request_stop(sim);
		if (id || sim_should_stop(sim))
		{
			wake_all_dongles(sim);
			return (NULL);
		}
		usleep(1000);
	}
}

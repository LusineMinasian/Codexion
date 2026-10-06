/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lminasia <lminasia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 15:43:48 by lminasia          #+#    #+#             */
/*   Updated: 2026/10/06 15:43:48 by lminasia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	coder_setup(t_coder *c, int i, t_sim *sim)
{
	int	right;

	right = (i + 1) % sim->number_of_coders;
	c->id = i + 1;
	c->first = i;
	c->second = right;
	if (right < i)
	{
		c->first = right;
		c->second = i;
	}
	c->compile_count = 0;
	c->last_compile_start = 0;
	c->sim = sim;
}

int	coders_init(t_sim *sim)
{
	int	i;

	sim->coders = malloc(sizeof(t_coder) * sim->number_of_coders);
	if (!sim->coders)
	{
		fprintf(stderr, "codexion: error\n");
		return (0);
	}
	i = 0;
	while (i < sim->number_of_coders)
	{
		coder_setup(&sim->coders[i], i, sim);
		if (pthread_mutex_init(&sim->coders[i].mutex, NULL) != 0)
		{
			while (--i >= 0)
				pthread_mutex_destroy(&sim->coders[i].mutex);
			free(sim->coders);
			sim->coders = NULL;
			fprintf(stderr, "codexion: mutex init failed\n");
			return (0);
		}
		i++;
	}
	return (1);
}

void	coders_destroy(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_destroy(&sim->coders[i].mutex);
		i++;
	}
	free(sim->coders);
	sim->coders = NULL;
}

static int	compile_once(t_coder *c)
{
	t_sim	*sim;

	sim = c->sim;
	if (!dongles_take(&sim->dongles[c->first], &sim->dongles[c->second], c))
		return (0);
	pthread_mutex_lock(&c->mutex);
	c->last_compile_start = time_now_ms(sim);
	pthread_mutex_unlock(&c->mutex);
	log_event(sim, c->id, "has taken a dongle");
	log_event(sim, c->id, "has taken a dongle");
	log_event(sim, c->id, "is compiling");
	sim_sleep(sim, sim->time_to_compile);
	pthread_mutex_lock(&c->mutex);
	c->compile_count = c->compile_count + 1;
	pthread_mutex_unlock(&c->mutex);
	dongle_release(&sim->dongles[c->first]);
	dongle_release(&sim->dongles[c->second]);
	return (1);
}

/*
** A lone coder has a single dongle and can never compile: it takes the
** dongle and waits for the monitor to declare the burnout.
** Even coders start half a compile late so neighbours don't all queue at once.
*/
void	*coder_routine(void *arg)
{
	t_coder	*c;
	t_sim	*sim;

	c = arg;
	sim = c->sim;
	if (c->first == c->second)
	{
		log_event(sim, c->id, "has taken a dongle");
		while (!sim_should_stop(sim))
			usleep(500);
		return (NULL);
	}
	if (c->id % 2 == 0)
		sim_sleep(sim, sim->time_to_compile / 2);
	while (!sim_should_stop(sim))
	{
		if (!compile_once(c))
			return (NULL);
		log_event(sim, c->id, "is debugging");
		sim_sleep(sim, sim->time_to_debug);
		log_event(sim, c->id, "is refactoring");
		sim_sleep(sim, sim->time_to_refactor);
	}
	return (NULL);
}
